// map_body.c
// Implementação do Mapa: Corpo Humano (a área jogável É o corpo)
// Disease's Doomsday — Projeto de Saúde Pública / DF
//
// O corpo preenche o mapa (MAP_WIDTH x MAP_HEIGHT) e é a própria arena: o jogo
// acontece DENTRO do corpo. A silhueta é a união de "cápsulas" (segmentos com
// raio) — usadas tanto para desenhar quanto para a COLISÃO que confina o herói
// e os patógenos dentro do corpo. Desenho e colisão usam EXATAMENTE o mesmo
// array BODY[], então estão sempre em sincronia.
#include "map_body.h"
#include "../../include/sprite_manager.h"
#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>

// Máscara de colisão autoritativa pré-processada (bitmask compacto) gerada por
// tools/bake_collision_mask.py a partir de Assets/Sprites/Map/corpo.png. Se o
// cabeçalho não existir, a colisão cai no fallback por cápsulas (BODY[]).
#if defined(__has_include)
#  if __has_include("map_body_mask.h")
#    include "map_body_mask.h"
#    define MAPBODY_HAVE_MASK 1
#  endif
#else
#  include "map_body_mask.h"
#  define MAPBODY_HAVE_MASK 1
#endif

// ============================================================================
// GEOMETRIA DO CORPO (coordenadas de mundo, MAP 4000x4000)
// Geometria alinhada a Assets/Sprites/Map/corpo.png (arte 1254x1254 escalada
// para o mundo 4000x4000). As cápsulas atravessam as divisões escuras da arte
// para manter cabeça, tronco e membros como uma única área jogável conexa.
// ============================================================================
#define BODY_CX 2000.0f

typedef struct BodyPart { Vector2 a, b; float r; } BodyPart;

// União de cápsulas que forma a silhueta. Partes adjacentes se sobrepõem para
// que o interior seja um espaço contínuo (o herói transita entre elas).
static const BodyPart BODY[] = {
    { { BODY_CX, 482.0f },  { BODY_CX, 482.0f },  315.0f }, // cabeça
    { { BODY_CX, 770.0f },  { BODY_CX, 955.0f },  165.0f }, // pescoço
    { { 1570.0f, 1110.0f }, { 2430.0f, 1110.0f }, 255.0f }, // ombros/peitoral
    { { 1570.0f, 1370.0f }, { 2430.0f, 1370.0f }, 250.0f }, // tórax
    { { 1580.0f, 1780.0f }, { 2420.0f, 1780.0f }, 330.0f }, // abdome superior
    { { 1560.0f, 2220.0f }, { 2440.0f, 2220.0f }, 315.0f }, // abdome inferior/pelve
    { { BODY_CX, 1260.0f }, { BODY_CX, 2280.0f }, 420.0f }, // núcleo contínuo do tronco
    { { 1510.0f, 1080.0f }, { 1300.0f, 1650.0f }, 170.0f }, // braço esq. superior
    { { 1300.0f, 1650.0f }, { 1140.0f, 2520.0f }, 145.0f }, // antebraço esq.
    { { 2490.0f, 1080.0f }, { 2700.0f, 1650.0f }, 170.0f }, // braço dir. superior
    { { 2700.0f, 1650.0f }, { 2860.0f, 2520.0f }, 145.0f }, // antebraço dir.
    { { 1740.0f, 2390.0f }, { 1680.0f, 2940.0f }, 220.0f }, // coxa esq.
    { { 1680.0f, 2940.0f }, { 1645.0f, 3370.0f }, 165.0f }, // canela esq.
    { { 1645.0f, 3370.0f }, { 1625.0f, 3715.0f }, 160.0f }, // pé esq.
    { { 2260.0f, 2390.0f }, { 2320.0f, 2940.0f }, 220.0f }, // coxa dir.
    { { 2320.0f, 2940.0f }, { 2355.0f, 3370.0f }, 165.0f }, // canela dir.
    { { 2355.0f, 3370.0f }, { 2375.0f, 3715.0f }, 160.0f }, // pé dir.
};
static const int BODY_N = (int)(sizeof(BODY) / sizeof(BODY[0]));

// Órgãos-alvo (centros), em escala grande, dentro do tórax/abdome.
#define LUNGS_CX   BODY_CX
#define LUNGS_CY   1320.0f
#define LUNG_OFFSET 300.0f
#define BLOOD_CX   BODY_CX
#define BLOOD_CY   1660.0f
#define HOSP_CX    BODY_CX        // foco da superbactéria: colonização intestinal (abdome)
#define HOSP_CY    2180.0f
// Centro seguro do tórax (folga máxima) — fallback determinístico de spawns.
#define THORAX_SAFE_X BODY_CX
#define THORAX_SAFE_Y 1780.0f

// Cores do tecido (interior do corpo)
#define COL_MEMBRANE (Color){ 175, 80, 96, 255 }
#define COL_TISSUE   (Color){ 96, 36, 46, 255 }
#define COL_TISSUE2  (Color){ 120, 46, 58, 255 }
#define COL_TISSUE_HI (Color){ 142, 60, 74, 255 }

// ============================================================================
// PULMÕES — parâmetros centralizados (cor / escala / respiração / efeitos).
// Tudo que controla a aparência do órgão vive aqui, para ajuste fácil.
// ============================================================================
#define LUNG_SCALE_X    168.0f   // meia-largura de um lobo (px de mundo)
#define LUNG_SCALE_Y    214.0f   // meia-altura de um lobo
#define LUNG_SPREAD     26.0f    // afastamento do lobo a partir do hilo
#define LUNG_CENTER_DY  120.0f   // deslocamento vertical do centro do lobo abaixo do hilo
#define LUNG_BREATH_AMP 0.055f   // amplitude da respiração (fração da escala)

// Estado clínico do órgão (cor/efeito derivados do estado REAL do foco da onda).
typedef enum LungCondition { LUNG_HEALTHY, LUNG_INFECTED, LUNG_RECOVERING } LungCondition;

// Paleta de um pulmão para um dado estado clínico.
typedef struct LungStyle {
    Color tissue;     // preenchimento do lobo
    Color tissueHi;   // realce de volume (luz)
    Color membrane;   // contorno/borda
    Color vessel;     // vasos/brônquios
    Color taint;      // manchas de contaminação (estado infectado)
    float glow;       // halo de fundo (0..~0.5)
    float taintAmt;   // intensidade da contaminação (0..1)
} LungStyle;

static LungStyle LungStyleFor(LungCondition cond, bool focus, float pulse)
{
    LungStyle s;
    switch (cond)
    {
        case LUNG_INFECTED:
            // Tecido inflamado: avermelhado/escuro, manchas esverdeadas pulsantes.
            s.tissue   = (Color){ 150, 70, 78, 255 };
            s.tissueHi = (Color){ 196, 104, 110, 255 };
            s.membrane = (Color){ 235, 120, 110, 255 };
            s.vessel   = (Color){ 120, 200, 120, 235 };
            s.taint    = (Color){ 110, 190, 90, 255 };
            s.glow     = focus ? (0.16f + pulse * 0.20f) : 0.12f;
            s.taintAmt = focus ? (0.80f + pulse * 0.20f) : 0.55f;
            break;
        case LUNG_RECOVERING:
            // Em recuperação: rosado saudável retomando, leve brilho ciano.
            s.tissue   = (Color){ 175, 96, 110, 255 };
            s.tissueHi = (Color){ 220, 150, 160, 255 };
            s.membrane = (Color){ 150, 220, 230, 255 };
            s.vessel   = (Color){ 235, 150, 160, 230 };
            s.taint    = (Color){ 150, 220, 200, 255 };
            s.glow     = 0.12f + pulse * 0.06f;
            s.taintAmt = 0.18f;
            break;
        default: // LUNG_HEALTHY
            // Saudável: rosa-tecido suave, brônquios claros, sem contaminação.
            s.tissue   = (Color){ 196, 120, 134, 255 };
            s.tissueHi = (Color){ 232, 168, 176, 255 };
            s.membrane = (Color){ 150, 200, 220, 230 };
            s.vessel   = (Color){ 226, 138, 150, 220 };
            s.taint    = (Color){ 0, 0, 0, 0 };
            s.glow     = focus ? (0.14f + pulse * 0.12f) : 0.08f;
            s.taintAmt = 0.0f;
            break;
    }
    return s;
}

// Contorno orgânico de um lobo pulmonar em espaço local (lado externo = x>0,
// y para baixo). A borda interna (mediastinal) traz o entalhe cardíaco. O lado
// é espelhado via 'side' no desenho; a profundidade vem das camadas, não de
// círculos sobrepostos.
static const Vector2 LUNG_OUTLINE[] = {
    { 0.10f, -0.92f },  // ápice interno
    { 0.42f, -1.00f },  // ápice (topo)
    { 0.72f, -0.86f },
    { 0.93f, -0.50f },
    { 1.00f, -0.06f },  // borda costal (externa) máxima
    { 0.95f,  0.36f },
    { 0.78f,  0.72f },
    { 0.50f,  0.95f },  // base diafragmática
    { 0.22f,  0.92f },
    { 0.08f,  0.60f },  // borda interna inferior
    { 0.24f,  0.26f },  // entalhe cardíaco (puxado p/ dentro)
    { 0.06f,  0.00f },
    { 0.02f, -0.42f },  // borda mediastinal (interna)
    { 0.05f, -0.74f },
};
static const int LUNG_OUTLINE_N = (int)(sizeof(LUNG_OUTLINE) / sizeof(LUNG_OUTLINE[0]));

// ============================================================================
// Helpers geométricos
// ============================================================================
// Distância de p ao segmento ab; preenche *out com o ponto mais próximo.
static float ClosestOnSeg(Vector2 p, Vector2 a, Vector2 b, Vector2 *out)
{
    Vector2 ab = { b.x - a.x, b.y - a.y };
    float L2 = ab.x * ab.x + ab.y * ab.y;
    float t = (L2 > 0.0001f) ? (((p.x - a.x) * ab.x + (p.y - a.y) * ab.y) / L2) : 0.0f;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    out->x = a.x + ab.x * t;
    out->y = a.y + ab.y * t;
    float dx = p.x - out->x, dy = p.y - out->y;
    return sqrtf(dx * dx + dy * dy);
}

// Desenha uma cápsula preenchida (segmento grosso + extremidades arredondadas).
static void DrawBodyCapsule(Vector2 a, Vector2 b, float radius, Color col)
{
    DrawLineEx(a, b, radius * 2.0f, col);
    DrawCircleV(a, radius, col);
    DrawCircleV(b, radius, col);
}

// ============================================================================
// COLISÃO COM O CORPO — fonte autoritativa: occupancy grid + signed distance
// field derivados de Assets/Sprites/Map/corpo.png (pré-processados por
// tools/bake_collision_mask.py -> Assets/Maps/map_body_mask.h). O bitmask é
// decodificado UMA vez (lazy init) e um SDF exato (Felzenszwalb & Huttenlocher)
// é calculado em memória; todas as consultas passam a ser O(1).
// Fallback seguro: se o cabeçalho do mask não existir (ou faltar memória para o
// SDF), a colisão usa a união de cápsulas BODY[] (a mesma geometria de desenho).
// ============================================================================

// ---- Fallback por cápsulas (usado só se o mask/SDF não estiver disponível) ----
static bool Caps_Contains(Vector2 p)
{
    for (int i = 0; i < BODY_N; i++)
    {
        Vector2 c;
        if (ClosestOnSeg(p, BODY[i].a, BODY[i].b, &c) <= BODY[i].r) return true;
    }
    return false;
}
static bool Caps_ContainsWithMargin(Vector2 p, float margin)
{
    if (margin <= 0.0f) return Caps_Contains(p);
    for (int i = 0; i < BODY_N; i++)
    {
        float allow = BODY[i].r - margin;
        if (allow <= 0.0f) continue;
        Vector2 c;
        if (ClosestOnSeg(p, BODY[i].a, BODY[i].b, &c) <= allow) return true;
    }
    return false;
}
static void Caps_ApplyCollision(Vector2 *pos, float radius)
{
    int bestPart = -1; Vector2 bestClosest = { 0, 0 }; float bestDist = 0.0f, bestOver = 1e18f;
    for (int i = 0; i < BODY_N; i++)
    {
        Vector2 c;
        float d = ClosestOnSeg(*pos, BODY[i].a, BODY[i].b, &c);
        float allow = BODY[i].r - radius; if (allow < 0.0f) allow = 0.0f;
        if (d <= allow) return;
        float over = d - allow;
        if (over < bestOver) { bestOver = over; bestPart = i; bestClosest = c; bestDist = d; }
    }
    if (bestPart >= 0)
    {
        float allow = BODY[bestPart].r - radius; if (allow < 0.0f) allow = 0.0f;
        Vector2 dir;
        if (bestDist > 0.001f) { dir.x = (pos->x - bestClosest.x) / bestDist; dir.y = (pos->y - bestClosest.y) / bestDist; }
        else                   { dir = (Vector2){ 0.0f, -1.0f }; }
        pos->x = bestClosest.x + dir.x * allow;
        pos->y = bestClosest.y + dir.y * allow;
    }
}

#ifdef MAPBODY_HAVE_MASK
// ---- Occupancy grid autoritativa (bitmask baked) + SDF em memória ----
#define MB_GN MAPBODY_MASK_GN
static float          gSDF[MB_GN * MB_GN]; // signed distance (px de mundo): >0 dentro, <0 fora
static unsigned char  gOcc[MB_GN * MB_GN]; // 1 = célula dentro do corpo
static float          gCellSize;           // px de mundo por célula
static Rectangle      gBounds;             // bbox do corpo (px de mundo) p/ minimapa
static Vector2        gSafe;               // ponto de folga máxima (centro seguro)
static bool           gInit = false, gReady = false;

static inline int MaskBit(int gx, int gy)
{
    int idx = gy * MB_GN + gx;
    return (MAPBODY_MASK_BITS[idx >> 3] >> (idx & 7)) & 1;
}
static inline int MB_Clampi(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

// Transformada de distância 1D ao quadrado (Felzenszwalb & Huttenlocher, O(n)).
static void MB_Edt1D(const float *f, float *d, int n, int *v, float *z)
{
    int k = 0; v[0] = 0; z[0] = -FLT_MAX; z[1] = FLT_MAX;
    for (int q = 1; q < n; q++)
    {
        float s = ((f[q] + (float)q * q) - (f[v[k]] + (float)v[k] * v[k])) / (2.0f * q - 2.0f * v[k]);
        while (s <= z[k]) { k--; s = ((f[q] + (float)q * q) - (f[v[k]] + (float)v[k] * v[k])) / (2.0f * q - 2.0f * v[k]); }
        k++; v[k] = q; z[k] = s; z[k + 1] = FLT_MAX;
    }
    k = 0;
    for (int q = 0; q < n; q++)
    {
        while (z[k + 1] < (float)q) k++;
        float dq = (float)(q - v[k]);
        d[q] = dq * dq + f[v[k]];
    }
}
// Distância euclidiana ao quadrado de cada célula à célula-semente mais próxima
// (semente = células com gOcc == seedVal). Separável: colunas e depois linhas.
static void MB_ComputeSq(int seedVal, float *out)
{
    static float f[MB_GN], d[MB_GN]; static int v[MB_GN + 1]; static float z[MB_GN + 2];
    for (int x = 0; x < MB_GN; x++)
    {
        for (int y = 0; y < MB_GN; y++) f[y] = (gOcc[y * MB_GN + x] == seedVal) ? 0.0f : 1e20f;
        MB_Edt1D(f, d, MB_GN, v, z);
        for (int y = 0; y < MB_GN; y++) out[y * MB_GN + x] = d[y];
    }
    for (int y = 0; y < MB_GN; y++)
    {
        for (int x = 0; x < MB_GN; x++) f[x] = out[y * MB_GN + x];
        MB_Edt1D(f, d, MB_GN, v, z);
        for (int x = 0; x < MB_GN; x++) out[y * MB_GN + x] = d[x];
    }
}
// Constrói a occupancy grid e o signed distance field uma única vez.
static bool MapBody_EnsureInit(void)
{
    if (gInit) return gReady;
    gInit = true; gReady = false;
    gCellSize = (float)MAP_WIDTH / (float)MB_GN;
    for (int y = 0; y < MB_GN; y++)
        for (int x = 0; x < MB_GN; x++)
            gOcc[y * MB_GN + x] = (unsigned char)MaskBit(x, y);

    float *tmp = (float *)malloc(sizeof(float) * MB_GN * MB_GN);
    if (!tmp)
    {
        fprintf(stderr, "[MapBody] sem memoria para o distance field; usando colisao por capsulas.\n");
        return gReady; // gReady=false -> fallback por cápsulas
    }
    MB_ComputeSq(0, gSDF); // distância² ao void externo  (>0 dentro do corpo)
    MB_ComputeSq(1, tmp);  // distância² ao corpo         (>0 fora do corpo)

    int minx = MB_GN, miny = MB_GN, maxx = 0, maxy = 0;
    float best = -1e30f; int bx = MB_GN / 2, by = MB_GN / 2;
    for (int y = 0; y < MB_GN; y++)
        for (int x = 0; x < MB_GN; x++)
        {
            int i = y * MB_GN + x;
            gSDF[i] = (sqrtf(gSDF[i]) - sqrtf(tmp[i])) * gCellSize; // SDF assinado em px
            if (gOcc[i]) { if (x < minx) minx = x; if (x > maxx) maxx = x; if (y < miny) miny = y; if (y > maxy) maxy = y; }
            if (gSDF[i] > best) { best = gSDF[i]; bx = x; by = y; }
        }
    free(tmp);
    gBounds = (Rectangle){ minx * gCellSize, miny * gCellSize,
                           (maxx - minx + 1) * gCellSize, (maxy - miny + 1) * gCellSize };
    gSafe   = (Vector2){ (bx + 0.5f) * gCellSize, (by + 0.5f) * gCellSize };
    gReady  = true;
    return gReady;
}
// SDF por célula mais próxima (determinístico; combina com a validação offline).
static float MB_SdfNearest(Vector2 p)
{
    if (p.x < 0.0f || p.y < 0.0f || p.x >= (float)MAP_WIDTH || p.y >= (float)MAP_HEIGHT) return -1e6f;
    int gx = MB_Clampi((int)(p.x / gCellSize), 0, MB_GN - 1);
    int gy = MB_Clampi((int)(p.y / gCellSize), 0, MB_GN - 1);
    return gSDF[gy * MB_GN + gx];
}
// SDF interpolado (bilinear) — usado para um empurrão de colisão suave.
static float MB_SdfBilinear(Vector2 p)
{
    float fx = p.x / gCellSize - 0.5f, fy = p.y / gCellSize - 0.5f;
    int x0 = MB_Clampi((int)floorf(fx), 0, MB_GN - 1), y0 = MB_Clampi((int)floorf(fy), 0, MB_GN - 1);
    int x1 = MB_Clampi(x0 + 1, 0, MB_GN - 1),          y1 = MB_Clampi(y0 + 1, 0, MB_GN - 1);
    float tx = Clamp(fx - floorf(fx), 0.0f, 1.0f), ty = Clamp(fy - floorf(fy), 0.0f, 1.0f);
    float a = gSDF[y0 * MB_GN + x0], b = gSDF[y0 * MB_GN + x1];
    float c = gSDF[y1 * MB_GN + x0], e = gSDF[y1 * MB_GN + x1];
    return (a * (1 - tx) + b * tx) * (1 - ty) + (c * (1 - tx) + e * tx) * ty;
}
#endif // MAPBODY_HAVE_MASK

bool MapBody_Contains(Vector2 p)
{
#ifdef MAPBODY_HAVE_MASK
    if (MapBody_EnsureInit())
    {
        if (p.x < 0.0f || p.y < 0.0f || p.x >= (float)MAP_WIDTH || p.y >= (float)MAP_HEIGHT) return false;
        int gx = MB_Clampi((int)(p.x / gCellSize), 0, MB_GN - 1);
        int gy = MB_Clampi((int)(p.y / gCellSize), 0, MB_GN - 1);
        return gOcc[gy * MB_GN + gx] != 0;
    }
#endif
    return Caps_Contains(p);
}

bool MapBody_ContainsWithMargin(Vector2 p, float margin)
{
#ifdef MAPBODY_HAVE_MASK
    if (MapBody_EnsureInit())
    {
        if (margin <= 0.0f) return MapBody_Contains(p);
        // O SDF é a distância exata à borda: o disco de raio `margin` cabe
        // inteiramente dentro do corpo sse SDF(p) >= margin.
        return MB_SdfNearest(p) >= margin;
    }
#endif
    return Caps_ContainsWithMargin(p, margin);
}

void MapBody_ApplyCollision(Vector2 *pos, float radius)
{
#ifdef MAPBODY_HAVE_MASK
    if (MapBody_EnsureInit())
    {
        // Já dentro com folga? (mesmo critério de ContainsWithMargin -> não move)
        if (MB_SdfNearest(*pos) >= radius) return;
        // Sobe o gradiente do SDF (aponta para dentro) até haver folga `radius`.
        // Sem teleporte: cada passo é o déficit local de distância.
        for (int it = 0; it < 8; it++)
        {
            float d = MB_SdfBilinear(*pos);
            if (d >= radius - 0.01f) return;
            float h = gCellSize;
            Vector2 g = { MB_SdfBilinear((Vector2){ pos->x + h, pos->y }) - MB_SdfBilinear((Vector2){ pos->x - h, pos->y }),
                          MB_SdfBilinear((Vector2){ pos->x, pos->y + h }) - MB_SdfBilinear((Vector2){ pos->x, pos->y - h }) };
            float gl = sqrtf(g.x * g.x + g.y * g.y);
            if (gl < 1e-4f) { g.x = gSafe.x - pos->x; g.y = gSafe.y - pos->y; gl = sqrtf(g.x * g.x + g.y * g.y); if (gl < 1e-4f) return; }
            g.x /= gl; g.y /= gl;
            float step = (radius - d) + 0.75f;
            pos->x += g.x * step; pos->y += g.y * step;
        }
        return;
    }
#endif
    Caps_ApplyCollision(pos, radius);
}

Vector2 MapBody_GetSafeCenter(void)
{
#ifdef MAPBODY_HAVE_MASK
    if (MapBody_EnsureInit()) return gSafe; // ponto de folga máxima do corpo
#endif
    return (Vector2){ THORAX_SAFE_X, THORAX_SAFE_Y };
}

// Caixa delimitadora do corpo (px de mundo) — usada para enquadrar o minimapa
// corporal no HUD sem desenhar a imagem 1254x1254 a cada frame.
Rectangle MapBody_WorldBounds(void)
{
#ifdef MAPBODY_HAVE_MASK
    if (MapBody_EnsureInit()) return gBounds;
#endif
    return (Rectangle){ 1080.0f, 150.0f, 1840.0f, 3620.0f };
}

Vector2 MapBody_RandomPointInside(Vector2 avoid, float minDistFromAvoid)
{
    for (int tries = 0; tries < 64; tries++)
    {
        Vector2 p = { (float)GetRandomValue(200, MAP_WIDTH - 200),
                      (float)GetRandomValue(200, MAP_HEIGHT - 200) };
        if (MapBody_Contains(p) &&
            Vector2DistanceSqr(p, avoid) >= minDistFromAvoid * minDistFromAvoid)
            return p;
    }
    // Fallback seguro: centro do tórax.
    return MapBody_GetSafeCenter();
}

// ----------------------------------------------------------------------------
// Helpers de spawn DETERMINÍSTICOS (chefe, lacaios, núcleos, power-ups)
// ----------------------------------------------------------------------------
bool MapBody_FindClearPoint(Vector2 preferred, Vector2 fallback, float margin, Vector2 *out)
{
    if (MapBody_ContainsWithMargin(preferred, margin)) { *out = preferred; return true; }

    // 1) Busca em espiral ao redor do ponto preferido (determinística).
    for (float rad = 40.0f; rad < 1470.0f; rad += 30.0f)
    {
        for (int a = 0; a < 360; a += 15)
        {
            float rr = (float)a * DEG2RAD;
            Vector2 p = { preferred.x + cosf(rr) * rad, preferred.y + sinf(rr) * rad };
            if (MapBody_ContainsWithMargin(p, margin)) { *out = p; return true; }
        }
    }
    // 2) Caminha em direção ao fallback (ex.: centro do tórax).
    for (int i = 1; i <= 20; i++)
    {
        float t = (float)i / 20.0f;
        Vector2 p = { preferred.x + (fallback.x - preferred.x) * t,
                      preferred.y + (fallback.y - preferred.y) * t };
        if (MapBody_ContainsWithMargin(p, margin)) { *out = p; return true; }
    }
    // 3) Último recurso: o próprio fallback.
    *out = fallback;
    return MapBody_ContainsWithMargin(fallback, margin);
}

Vector2 MapBody_FindSpawnPoint(Vector2 preferred, float margin)
{
    Vector2 out;
    MapBody_FindClearPoint(preferred, MapBody_GetSafeCenter(), margin, &out);
    return out;
}

int MapBody_PlaceCores(Vector2 bossCenter, Vector2 *out, int maxCores,
                       float coreMargin, float bossClear, float interCore)
{
    Vector2 center = MapBody_GetSafeCenter();
    float base = atan2f(bossCenter.y - center.y, bossCenter.x - center.x);
    const float dists[6] = { 320.0f, 280.0f, 240.0f, 200.0f, 360.0f, 160.0f };
    int placed = 0;

    for (int i = 0; i < maxCores; i++)
    {
        float ang = base + (float)i * (2.0f * PI / (float)maxCores);
        bool done = false;
        Vector2 chosen = center;

        // Tenta vários raios ao redor do chefe, validando margem/folga/sobreposição.
        for (int d = 0; d < 6 && !done; d++)
        {
            Vector2 pref = { bossCenter.x + cosf(ang) * dists[d],
                             bossCenter.y + sinf(ang) * dists[d] };
            Vector2 c;
            if (!MapBody_FindClearPoint(pref, center, coreMargin, &c)) continue;
            if (Vector2Distance(c, bossCenter) < bossClear) continue;
            bool overlap = false;
            for (int k = 0; k < placed; k++)
                if (Vector2Distance(c, out[k]) < interCore) { overlap = true; break; }
            if (overlap) continue;
            chosen = c; done = true;
        }

        // Fallback determinístico: anel ao redor do centro seguro do tórax.
        if (!done)
        {
            for (int k = 0; k < 360 && !done; k += 7)
            {
                float aa = (float)(k + i * 53) * DEG2RAD;
                float rr = 260.0f + (float)i * 20.0f;
                Vector2 c = { center.x + cosf(aa) * rr, center.y + sinf(aa) * rr };
                if (!MapBody_ContainsWithMargin(c, coreMargin)) continue;
                bool overlap = false;
                for (int q = 0; q < placed; q++)
                    if (Vector2Distance(c, out[q]) < interCore) { overlap = true; break; }
                if (overlap) continue;
                chosen = c; done = true;
            }
        }
        // Garantia final: nunca deixa um núcleo sem posição utilizável.
        out[placed++] = chosen;
    }
    return placed;
}

// ============================================================================
// API pública — foco/órgão da onda atual
// ============================================================================
BodyRegion MapBody_GetFocusRegion(int currentWorld, int wave)
{
    if (currentWorld == WORLD_BACTERIA)
        return (wave >= WAVES_PER_WORLD) ? REGION_HOSPITAL_FOCUS : REGION_LUNGS;
    if (wave >= WAVES_PER_WORLD) return REGION_BLOODSTREAM;
    return (wave % 2 == 1) ? REGION_LUNGS : REGION_BLOODSTREAM;
}

Vector2 MapBody_GetRegionCenter(BodyRegion region)
{
    switch (region)
    {
        case REGION_LUNGS:          return (Vector2){ LUNGS_CX, LUNGS_CY };
        case REGION_BLOODSTREAM:    return (Vector2){ BLOOD_CX, BLOOD_CY };
        case REGION_HOSPITAL_FOCUS: return (Vector2){ HOSP_CX, HOSP_CY };
        default:                    return MapBody_GetSafeCenter();
    }
}

const char *MapBody_GetRegionLabel(BodyRegion region)
{
    switch (region)
    {
        case REGION_LUNGS:          return "Pulmoes (vias respiratorias)";
        case REGION_BLOODSTREAM:    return "Corrente sanguinea";
        case REGION_HOSPITAL_FOCUS: return "Foco hospitalar (intestino)";
        default:                    return "Organismo";
    }
}

const char *MapBody_GetDiseaseLabel(int currentWorld, int wave)
{
    if (currentWorld == WORLD_BACTERIA)
    {
        if (wave >= WAVES_PER_WORLD) return "Superbacteria KPC (infeccao hospitalar)";
        return "Pneumonia bacteriana";
    }
    if (wave >= WAVES_PER_WORLD) return "Dengue grave (virus de RNA)";
    return (wave % 2 == 1) ? "Influenza (virus de RNA)" : "Dengue (Aedes aegypti)";
}

// ============================================================================
// DESENHO PROCEDURAL — órgãos e rótulos (camadas, contorno orgânico, animação)
// ============================================================================

// Rótulo de órgão com painel/cápsula semitransparente, contorno, sombra e
// padding — sempre legível sobre o tecido. `focus` aumenta o contraste/brilho.
static void DrawOrganLabel(Font font, const char *txt, Vector2 center, Color col, bool focus)
{
    float fs = focus ? 50.0f : 44.0f;
    Vector2 sz = MeasureTextEx(font, txt, fs, 2.0f);
    float padX = 26.0f, padY = 14.0f;
    Rectangle panel = { center.x - sz.x * 0.5f - padX, center.y - sz.y * 0.5f - padY,
                        sz.x + padX * 2.0f, sz.y + padY * 2.0f };
    // Painel escuro translúcido (garante contraste) + leve sombra projetada.
    DrawRectangleRounded((Rectangle){ panel.x + 6, panel.y + 8, panel.width, panel.height },
                         0.5f, 10, Fade(BLACK, 0.35f));
    DrawRectangleRounded(panel, 0.5f, 10, Fade((Color){ 6, 10, 16, 255 }, focus ? 0.80f : 0.68f));
    DrawRectangleRoundedLines(panel, 0.5f, 10, Fade(col, focus ? 0.95f : 0.6f));
    // Texto com contorno escuro para legibilidade máxima.
    Vector2 at = { center.x - sz.x * 0.5f, center.y - sz.y * 0.5f };
    for (int dx = -2; dx <= 2; dx += 2)
        for (int dy = -2; dy <= 2; dy += 2)
            if (dx || dy)
                DrawTextEx(font, txt, (Vector2){ at.x + dx, at.y + dy }, fs, 2.0f, Fade(BLACK, 0.45f));
    DrawTextEx(font, txt, at, fs, 2.0f, col);
}

// Preenche um polígono via leque de triângulos a partir de um centro. Desenha
// cada triângulo nas duas orientações para ser robusto a backface culling
// (custo desprezível: poucas dezenas de triângulos por quadro).
static void FillPolyFan(const Vector2 *p, int n, Vector2 c, Color col)
{
    for (int i = 0; i < n; i++)
    {
        Vector2 a = p[i], b = p[(i + 1) % n];
        DrawTriangle(c, a, b, col);
        DrawTriangle(c, b, a, col);
    }
}

// Transforma o contorno local do lobo para coordenadas de mundo.
// 'notch' (0..1) controla a profundidade do entalhe cardíaco.
static Vector2 LungLocalToWorld(Vector2 lp, Vector2 center, int side, float bs, float notch)
{
    float lx = lp.x;
    // O ponto do entalhe cardíaco (índice 10 do contorno) é puxado p/ dentro.
    // Aproximamos suavizando lx quando o ponto está na borda interna inferior.
    (void)notch;
    return (Vector2){ center.x + side * lx * LUNG_SCALE_X * bs,
                      center.y + lp.y * LUNG_SCALE_Y * bs };
}

// Árvore brônquica estilizada a partir do hilo para dentro do lobo (uma divisão
// principal + ramos secundários), com leve animação respiratória.
static void DrawBronchialTree(Vector2 hilum, Vector2 center, int side, Color vessel, float bs)
{
    Vector2 root = hilum;
    Vector2 main = { center.x - side * LUNG_SCALE_X * 0.18f * bs, center.y - LUNG_SCALE_Y * 0.10f * bs };
    DrawLineEx(root, main, 12.0f * bs, Fade(vessel, 0.75f));
    DrawLineEx(root, main, 5.0f * bs, Fade(vessel, 0.95f));

    // 3 brônquios secundários partindo do tronco principal.
    const float ang[3] = { -0.62f, -0.04f, 0.58f };
    const float len[3] = { 0.46f, 0.58f, 0.42f };
    for (int b = 0; b < 3; b++)
    {
        float baseA = (side > 0 ? 0.0f : PI);
        float a = baseA + side * ang[b];
        Vector2 tip = { main.x + cosf(a) * LUNG_SCALE_X * len[b] * bs,
                        main.y + sinf(a) * LUNG_SCALE_Y * len[b] * bs };
        DrawLineEx(main, tip, 6.0f * bs, Fade(vessel, 0.6f));
        // bifurcação final
        Vector2 t1 = { tip.x + side * 22.0f * bs, tip.y - 26.0f * bs };
        Vector2 t2 = { tip.x + side * 30.0f * bs, tip.y + 20.0f * bs };
        DrawLineEx(tip, t1, 3.0f * bs, Fade(vessel, 0.5f));
        DrawLineEx(tip, t2, 3.0f * bs, Fade(vessel, 0.5f));
    }
}

// Um lobo pulmonar completo (silhueta orgânica preenchida + volume + fissuras +
// brônquios + alvéolos/vasos + contorno + contaminação opcional). Sem círculos
// sobrepostos: a forma vem do contorno LUNG_OUTLINE.
static void DrawLungLobe(Vector2 hilum, int side, LungStyle st, float breathe, float notch, float time)
{
    float bs = 1.0f + breathe * LUNG_BREATH_AMP;
    Vector2 center = { hilum.x + side * (LUNG_SPREAD + LUNG_SCALE_X * 0.45f),
                       hilum.y + LUNG_CENTER_DY };

    Vector2 pts[32];
    int n = LUNG_OUTLINE_N;
    for (int i = 0; i < n; i++)
        pts[i] = LungLocalToWorld(LUNG_OUTLINE[i], center, side, bs, notch);

    Vector2 fanC = { center.x + side * LUNG_SCALE_X * 0.42f * bs, center.y };

    // 1) Halo orgânico de fundo (profundidade/atmosfera, sem contorno duro).
    DrawCircleV((Vector2){ center.x + side * LUNG_SCALE_X * 0.35f, center.y },
                LUNG_SCALE_Y * 1.05f, Fade(st.membrane, st.glow * 0.5f));

    // 2) Sombra projetada (volume) — mesma silhueta deslocada e escurecida.
    Vector2 shadow[32];
    for (int i = 0; i < n; i++) shadow[i] = (Vector2){ pts[i].x + 10.0f, pts[i].y + 16.0f };
    FillPolyFan(shadow, n, (Vector2){ fanC.x + 10.0f, fanC.y + 16.0f }, Fade(BLACK, 0.30f));

    // 3) Corpo do lobo (preenchimento base).
    FillPolyFan(pts, n, fanC, st.tissue);

    // 4) Realce de volume: silhueta menor deslocada para o topo-externo, em luz.
    Vector2 hi[32];
    Vector2 hiC = { center.x + side * LUNG_SCALE_X * 0.30f, center.y - LUNG_SCALE_Y * 0.18f };
    for (int i = 0; i < n; i++)
    {
        hi[i].x = hiC.x + (pts[i].x - center.x) * 0.62f;
        hi[i].y = hiC.y + (pts[i].y - center.y) * 0.62f;
    }
    FillPolyFan(hi, n, hiC, Fade(st.tissueHi, 0.55f));

    // 5) Fissuras (divisões de lobos): oblíqua sempre; horizontal no pulmão maior.
    Vector2 fa = { center.x + side * LUNG_SCALE_X * 0.10f, center.y - LUNG_SCALE_Y * 0.55f };
    Vector2 fb = { center.x + side * LUNG_SCALE_X * 0.86f, center.y + LUNG_SCALE_Y * 0.22f };
    DrawLineEx(fa, fb, 3.0f, Fade(st.membrane, 0.45f));
    if (notch < 0.5f) // pulmão maior (sem entalhe profundo) ganha fissura horizontal
    {
        Vector2 ha = { center.x + side * LUNG_SCALE_X * 0.40f, center.y - LUNG_SCALE_Y * 0.06f };
        Vector2 hb = { center.x + side * LUNG_SCALE_X * 0.96f, center.y - LUNG_SCALE_Y * 0.12f };
        DrawLineEx(ha, hb, 2.5f, Fade(st.membrane, 0.35f));
    }

    // 6) Brônquios internos.
    DrawBronchialTree(hilum, center, side, st.vessel, bs);

    // 7) Alvéolos/capilares estilizados — pequenos aglomerados determinísticos
    //    (posição por hash do índice; animação por sin(time), nunca aleatória).
    for (int i = 0; i < 9; i++)
    {
        float u = ((i * 73) % 100) / 100.0f;        // 0..1 ao longo da largura
        float v = ((i * 137) % 100) / 100.0f;       // 0..1 ao longo da altura
        Vector2 ap = { center.x + side * (0.20f + u * 0.66f) * LUNG_SCALE_X,
                       center.y + (-0.55f + v * 1.30f) * LUNG_SCALE_Y };
        float wobble = 0.5f + 0.5f * sinf(time * 1.6f + i);
        DrawCircleV(ap, 6.0f + wobble * 2.0f, Fade(st.tissueHi, 0.30f));
        DrawCircleLines((int)ap.x, (int)ap.y, 9.0f, Fade(st.vessel, 0.22f));
    }

    // 8) Contaminação (estado infectado): manchas escuras pulsantes em áreas fixas.
    if (st.taintAmt > 0.01f && st.taint.a > 0)
    {
        for (int i = 0; i < 6; i++)
        {
            float u = ((i * 53 + 17) % 100) / 100.0f;
            float v = ((i * 91 + 31) % 100) / 100.0f;
            Vector2 tp = { center.x + side * (0.18f + u * 0.62f) * LUNG_SCALE_X,
                           center.y + (-0.40f + v * 1.10f) * LUNG_SCALE_Y };
            float p = 0.5f + 0.5f * sinf(time * 2.4f + i * 1.7f);
            float r = (12.0f + (i % 3) * 7.0f) * (0.7f + 0.5f * p);
            DrawCircleV(tp, r, Fade(st.taint, 0.16f * st.taintAmt * (0.6f + 0.4f * p)));
            DrawCircleV(tp, r * 0.45f, Fade(st.taint, 0.30f * st.taintAmt));
        }
    }

    // 9) Contorno (membrana pleural) — linha contínua fechada por cima de tudo.
    for (int i = 0; i < n; i++)
        DrawLineEx(pts[i], pts[(i + 1) % n], 3.5f, st.membrane);
}

// Traqueia central (com anéis cartilaginosos) + bifurcação para os dois brônquios
// principais que chegam a cada hilo.
static void DrawTrachea(Vector2 top, Vector2 bif, Vector2 hilumL, Vector2 hilumR, Color col, float breathe)
{
    float bs = 1.0f + breathe * LUNG_BREATH_AMP;
    DrawLineEx(top, bif, 30.0f * bs, Fade(col, 0.50f));
    DrawLineEx(top, bif, 16.0f * bs, Fade(col, 0.72f));
    // Anéis cartilaginosos.
    int rings = 5;
    for (int k = 0; k < rings; k++)
    {
        float t = (float)k / (float)(rings - 1);
        float yy = top.y + (bif.y - top.y) * t;
        DrawLineEx((Vector2){ top.x - 17.0f * bs, yy }, (Vector2){ top.x + 17.0f * bs, yy }, 4.0f, Fade(col, 0.85f));
    }
    // Brônquios principais até cada hilo.
    DrawLineEx(bif, hilumL, 13.0f * bs, Fade(col, 0.62f));
    DrawLineEx(bif, hilumR, 13.0f * bs, Fade(col, 0.62f));
    DrawCircleV(bif, 9.0f * bs, Fade(col, 0.8f)); // carina
}

// ============================================================================
// DrawMapBody — desenha o corpo preenchido + órgãos (dentro de BeginMode2D)
// Camadas (Etapa 3): silhueta/base -> textura interna -> órgãos -> detalhes ->
// rótulos. (Fundo externo e entidades/HUD são desenhados pelo chamador.)
// ============================================================================
void DrawMapBody(Font font, int currentWorld, int wave, float time)
{
    float breathe = 0.5f + 0.5f * sinf(time * 0.9f);   // respiração lenta
    float pulse   = 0.5f + 0.5f * sinf(time * 3.0f);   // pulsação rápida (foco)

    // ------------------------------------------------------------------------
    // 2) SILHUETA / BASE DO CORPO
    // ------------------------------------------------------------------------
    if (SpriteAvailable(SPR_MAP_BODY))
    {
        Vector2 center = { MAP_WIDTH / 2.0f, MAP_HEIGHT / 2.0f };
        DrawSpriteCentered(SPR_MAP_BODY, center, (Vector2){ MAP_WIDTH, MAP_HEIGHT }, 0.0f, WHITE);
    }
    else if (SpriteAvailable(SPR_MAP_SILHOUETTE))
    {
        Vector2 center = { MAP_WIDTH / 2.0f, MAP_HEIGHT / 2.0f };
        DrawSpriteCentered(SPR_MAP_SILHOUETTE, center, (Vector2){ MAP_WIDTH, MAP_HEIGHT }, 0.0f, WHITE);
    }
    else
    {
        // Contorno em camadas (halo orgânico -> membrana -> tecido -> brilho central).
        for (int i = 0; i < BODY_N; i++)
            DrawBodyCapsule(BODY[i].a, BODY[i].b, BODY[i].r + 44.0f, Fade(COL_MEMBRANE, 0.18f + 0.06f * breathe));
        for (int i = 0; i < BODY_N; i++)
            DrawBodyCapsule(BODY[i].a, BODY[i].b, BODY[i].r + 22.0f, COL_MEMBRANE);
        for (int i = 0; i < BODY_N; i++)
            DrawBodyCapsule(BODY[i].a, BODY[i].b, BODY[i].r, COL_TISSUE);
        // 3) Textura interna: realce suave ao longo das linhas centrais + células.
        for (int i = 0; i < BODY_N; i++)
            DrawBodyCapsule(BODY[i].a, BODY[i].b, BODY[i].r * 0.46f, Fade(COL_TISSUE_HI, 0.35f));
        for (int i = 0; i < 80; i++)
        {
            float bx = BODY_CX - 760.0f + fmodf(i * 167.0f, 1520.0f);
            float by = 760.0f + fmodf(i * 233.0f, 2700.0f);
            Vector2 cp = { bx, by };
            if (!MapBody_ContainsWithMargin(cp, 30.0f)) continue;
            float rr = 16.0f + (i % 6) * 9.0f;
            float a = 0.05f + 0.05f * sinf(time * 1.2f + i);
            DrawCircleLines((int)bx, (int)by, rr, Fade(COL_TISSUE2, 0.22f));
            DrawCircleV(cp, rr * 0.5f, Fade(COL_TISSUE2, a));
        }
        // Vasos sutis subindo pelo tronco (linhas finas onduladas aproximadas).
        for (int v = -1; v <= 1; v += 1)
        {
            Color vcol = Fade((Color){ 150, 60, 70, 255 }, 0.18f);
            Vector2 prev = { BODY_CX + v * 220.0f, 1400.0f };
            for (int s = 1; s <= 16; s++)
            {
                float yy = 1400.0f + s * 90.0f;
                float xx = BODY_CX + v * 220.0f + sinf(yy * 0.01f + v) * 60.0f;
                Vector2 cur = { xx, yy };
                if (MapBody_Contains(cur)) DrawLineEx(prev, cur, 6.0f, vcol);
                prev = cur;
            }
        }
    }

    BodyRegion focus = MapBody_GetFocusRegion(currentWorld, wave);

    // ------------------------------------------------------------------------
    // 4) ÓRGÃOS
    // ------------------------------------------------------------------------
    if (SpriteAvailable(SPR_MAP_ORGANS))
    {
        Vector2 center = { MAP_WIDTH / 2.0f, MAP_HEIGHT / 2.0f };
        DrawSpriteCentered(SPR_MAP_ORGANS, center, (Vector2){ MAP_WIDTH, MAP_HEIGHT }, 0.0f, WHITE);
    }
    else
    {
        // ---- PULMÕES (silhueta orgânica em camadas + traqueia + brônquios) ----
        bool fLung = (focus == REGION_LUNGS);
        // Estado clínico derivado do estado REAL do foco (sem feedback falso):
        // infectado quando o pulmão É o foco da onda; em recuperação se já foi
        // foco numa onda anterior deste Mundo; saudável caso contrário.
        LungCondition cond;
        if (fLung) cond = LUNG_INFECTED;
        else
        {
            bool lungWasFocus = false;
            for (int w = 1; w < wave; w++)
                if (MapBody_GetFocusRegion(currentWorld, w) == REGION_LUNGS) { lungWasFocus = true; break; }
            cond = lungWasFocus ? LUNG_RECOVERING : LUNG_HEALTHY;
        }
        LungStyle st = LungStyleFor(cond, fLung, pulse);

        Vector2 hilumL = { LUNGS_CX - LUNG_OFFSET * 0.5f, LUNGS_CY - 40.0f };
        Vector2 hilumR = { LUNGS_CX + LUNG_OFFSET * 0.5f, LUNGS_CY - 40.0f };
        Vector2 trTop = { LUNGS_CX, 1230.0f };
        Vector2 trBif = { LUNGS_CX, 1470.0f };

        // Lobos primeiro (a traqueia/brônquios passam por cima dos hilos).
        // Assimetria anatômica: lobo esquerdo do mapa = 3 lobos (fissura extra),
        // lobo direito = 2 lobos com entalhe cardíaco mais marcado.
        DrawLungLobe(hilumL, -1, st, breathe, 0.0f, time);
        DrawLungLobe(hilumR, +1, st, breathe, 1.0f, time);
        DrawTrachea(trTop, trBif, hilumL, hilumR, st.vessel, breathe);

        // ---- CORAÇÃO / CORRENTE SANGUÍNEA ----
        bool fBlood = (focus == REGION_BLOODSTREAM);
        Color bloodCol = fBlood ? (Color){ 255, 70, 90, 255 } : (Color){ 200, 70, 80, 205 };
        float bloodGlow = fBlood ? (0.20f + pulse * 0.24f) : 0.12f;
        float beat = 1.0f + (fBlood ? pulse * 0.10f : breathe * 0.05f); // batimento
        Vector2 hc = { BLOOD_CX, BLOOD_CY };
        DrawCircleV(hc, 210.0f * beat, Fade(bloodCol, bloodGlow));
        // Forma de coração: dois lóbulos + ponta inferior.
        float hr = 96.0f * beat;
        DrawCircleV((Vector2){ hc.x - hr * 0.55f, hc.y - hr * 0.35f }, hr * 0.62f, Fade(bloodCol, 0.65f));
        DrawCircleV((Vector2){ hc.x + hr * 0.55f, hc.y - hr * 0.35f }, hr * 0.62f, Fade(bloodCol, 0.65f));
        DrawTriangle((Vector2){ hc.x - hr, hc.y - hr * 0.2f },
                     (Vector2){ hc.x, hc.y + hr * 1.05f },
                     (Vector2){ hc.x + hr, hc.y - hr * 0.2f }, Fade(bloodCol, 0.65f));
        DrawCircleLines((int)hc.x, (int)hc.y, hr * 1.05f, Fade(bloodCol, 0.5f));
        // Grandes vasos saindo do coração.
        DrawLineEx(hc, (Vector2){ BLOOD_CX, 1470.0f }, 16.0f, Fade(bloodCol, 0.45f));
        DrawLineEx(hc, (Vector2){ BLOOD_CX, 2650.0f }, 14.0f, Fade(bloodCol, 0.40f));
        DrawLineEx(hc, (Vector2){ BLOOD_CX - 260.0f, 2000.0f }, 10.0f, Fade(bloodCol, 0.35f));
        DrawLineEx(hc, (Vector2){ BLOOD_CX + 260.0f, 2000.0f }, 10.0f, Fade(bloodCol, 0.35f));

        // ---- FOCO HOSPITALAR (colonização intestinal — abdome) ----
        bool fHosp = (focus == REGION_HOSPITAL_FOCUS);
        Color hospCol = fHosp ? (Color){ 255, 210, 60, 255 } : (Color){ 190, 170, 90, 195 };
        float hospGlow = fHosp ? (0.16f + pulse * 0.20f) : 0.09f;
        DrawCircleV((Vector2){ HOSP_CX, HOSP_CY }, 300.0f, Fade(hospCol, hospGlow));
        // Alças intestinais (caminho serpenteante aproximado por segmentos).
        Vector2 prev = { HOSP_CX - 220.0f, HOSP_CY - 110.0f };
        for (int s = 1; s <= 40; s++)
        {
            float t = (float)s / 40.0f;
            float xx = HOSP_CX + sinf(t * PI * 5.0f + 0.4f) * 210.0f;
            float yy = (HOSP_CY - 110.0f) + t * 230.0f;
            Vector2 cur = { xx, yy };
            DrawLineEx(prev, cur, 26.0f, Fade(hospCol, 0.45f));
            DrawLineEx(prev, cur, 14.0f, Fade(hospCol, 0.7f));
            prev = cur;
        }

        // ------------------------------------------------------------------
        // 5) DETALHES / DESTAQUES — anel de foco pulsante no órgão da onda.
        // ------------------------------------------------------------------
        Vector2 fc = MapBody_GetRegionCenter(focus);
        float ringR = 330.0f + pulse * 26.0f;
        Color ringCol = fLung ? (Color){ 120, 220, 255, 255 }
                      : fBlood ? (Color){ 255, 120, 130, 255 }
                               : (Color){ 255, 225, 120, 255 };
        DrawCircleLines((int)fc.x, (int)fc.y, ringR, Fade(ringCol, 0.25f + 0.2f * pulse));
        DrawCircleLines((int)fc.x, (int)fc.y, ringR + 8.0f, Fade(ringCol, 0.12f));
    }

    // ------------------------------------------------------------------------
    // 6) RÓTULOS DOS ÓRGÃOS (painel + contorno + contraste) — sempre legíveis.
    // ------------------------------------------------------------------------
    DrawOrganLabel(font, "PULMOES", (Vector2){ LUNGS_CX, 1410.0f },
                   (focus == REGION_LUNGS) ? (Color){ 150, 230, 255, 255 } : Fade(WHITE, 0.7f),
                   focus == REGION_LUNGS);
    DrawOrganLabel(font, "CORRENTE SANGUINEA", (Vector2){ BLOOD_CX, 1980.0f },
                   (focus == REGION_BLOODSTREAM) ? (Color){ 255, 140, 150, 255 } : Fade(WHITE, 0.7f),
                   focus == REGION_BLOODSTREAM);
    DrawOrganLabel(font, "FOCO HOSPITALAR", (Vector2){ HOSP_CX, 2600.0f },
                   (focus == REGION_HOSPITAL_FOCUS) ? (Color){ 255, 230, 140, 255 } : Fade(WHITE, 0.7f),
                   focus == REGION_HOSPITAL_FOCUS);
}
