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

// ============================================================================
// GEOMETRIA DO CORPO (coordenadas de mundo, MAP 4000x4000)
// O corpo é grande e centralizado (eixo x = 2000), de pé (cabeça em cima).
// Silhueta humana definida: cabeça, pescoço (estreito), ombros (largos), tórax,
// cintura, quadril, braços (2 segmentos) e pernas (2 segmentos). Cada cápsula
// se sobrepõe à vizinha para manter o interior contínuo (sem gargalos/becos).
// Validado: área interna ~5.5M px², 1 única região conexa, tudo dentro do mapa.
// ============================================================================
#define BODY_CX 2000.0f

typedef struct BodyPart { Vector2 a, b; float r; } BodyPart;

// União de cápsulas que forma a silhueta. Partes adjacentes se sobrepõem para
// que o interior seja um espaço contínuo (o herói transita entre elas).
static const BodyPart BODY[] = {
    { { BODY_CX, 500.0f },  { BODY_CX, 500.0f },  395.0f }, // cabeça (círculo)
    { { BODY_CX, 840.0f },  { BODY_CX, 1190.0f }, 185.0f }, // pescoço (estreito)
    { { 1420.0f, 1310.0f }, { 2580.0f, 1310.0f }, 320.0f }, // ombros (linha larga)
    { { BODY_CX, 1540.0f }, { BODY_CX, 2260.0f }, 640.0f }, // núcleo do tronco (alto)
    { { 1470.0f, 1500.0f }, { 2530.0f, 1500.0f }, 500.0f }, // largura do tórax
    { { 1480.0f, 2160.0f }, { 2520.0f, 2160.0f }, 575.0f }, // largura do abdome (barriga)
    { { BODY_CX, 2180.0f }, { BODY_CX, 2660.0f }, 490.0f }, // baixo-ventre (altura)
    { { 1600.0f, 2720.0f }, { 2400.0f, 2720.0f }, 450.0f }, // quadril/pelve (largo)
    { { 1650.0f, 1370.0f }, { 1320.0f, 2140.0f }, 265.0f }, // braço esq. (superior)
    { { 1320.0f, 2140.0f }, { 1190.0f, 2820.0f }, 235.0f }, // antebraço esq.
    { { 2350.0f, 1370.0f }, { 2680.0f, 2140.0f }, 265.0f }, // braço dir. (superior)
    { { 2680.0f, 2140.0f }, { 2810.0f, 2820.0f }, 235.0f }, // antebraço dir.
    { { 1870.0f, 2740.0f }, { 1775.0f, 3320.0f }, 290.0f }, // coxa esq.
    { { 1775.0f, 3320.0f }, { 1730.0f, 3680.0f }, 245.0f }, // canela esq.
    { { 2130.0f, 2740.0f }, { 2225.0f, 3320.0f }, 290.0f }, // coxa dir.
    { { 2225.0f, 3320.0f }, { 2270.0f, 3680.0f }, 245.0f }, // canela dir.
};
static const int BODY_N = (int)(sizeof(BODY) / sizeof(BODY[0]));

// Órgãos-alvo (centros), em escala grande, dentro do tórax/abdome.
#define LUNGS_CX   BODY_CX
#define LUNGS_CY   1600.0f
#define LUNG_OFFSET 360.0f
#define BLOOD_CX   BODY_CX
#define BLOOD_CY   1760.0f
#define HOSP_CX    BODY_CX        // foco da superbactéria: colonização intestinal (abdome)
#define HOSP_CY    2320.0f
// Centro seguro do tórax (folga máxima) — fallback determinístico de spawns.
#define THORAX_SAFE_X BODY_CX
#define THORAX_SAFE_Y 1700.0f

// Cores do tecido (interior do corpo)
#define COL_MEMBRANE (Color){ 175, 80, 96, 255 }
#define COL_TISSUE   (Color){ 96, 36, 46, 255 }
#define COL_TISSUE2  (Color){ 120, 46, 58, 255 }
#define COL_TISSUE_HI (Color){ 142, 60, 74, 255 }

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
// COLISÃO COM O CORPO
// ============================================================================
bool MapBody_Contains(Vector2 p)
{
    for (int i = 0; i < BODY_N; i++)
    {
        Vector2 c;
        if (ClosestOnSeg(p, BODY[i].a, BODY[i].b, &c) <= BODY[i].r) return true;
    }
    return false;
}

bool MapBody_ContainsWithMargin(Vector2 p, float margin)
{
    // Condição SUFICIENTE (conservadora): se o ponto está a <= (r - margin) de
    // alguma cápsula, então o disco de raio `margin` ao redor de p cabe inteiro
    // nessa cápsula => está totalmente dentro do corpo, longe das paredes.
    if (margin <= 0.0f) return MapBody_Contains(p);
    for (int i = 0; i < BODY_N; i++)
    {
        float allow = BODY[i].r - margin;
        if (allow <= 0.0f) continue;
        Vector2 c;
        if (ClosestOnSeg(p, BODY[i].a, BODY[i].b, &c) <= allow) return true;
    }
    return false;
}

void MapBody_ApplyCollision(Vector2 *pos, float radius)
{
    int bestPart = -1;
    Vector2 bestClosest = { 0, 0 };
    float bestDist = 0.0f;
    float bestOver = 1e18f;

    for (int i = 0; i < BODY_N; i++)
    {
        Vector2 c;
        float d = ClosestOnSeg(*pos, BODY[i].a, BODY[i].b, &c);
        float allow = BODY[i].r - radius;     // margem p/ o corpo do raio caber
        if (allow < 0.0f) allow = 0.0f;
        if (d <= allow) return;               // já está com folga dentro desta parte
        float over = d - allow;               // o quanto está fora desta parte
        if (over < bestOver) { bestOver = over; bestPart = i; bestClosest = c; bestDist = d; }
    }

    if (bestPart >= 0)
    {
        float allow = BODY[bestPart].r - radius;
        if (allow < 0.0f) allow = 0.0f;
        Vector2 dir;
        if (bestDist > 0.001f) { dir.x = (pos->x - bestClosest.x) / bestDist; dir.y = (pos->y - bestClosest.y) / bestDist; }
        else                   { dir = (Vector2){ 0.0f, -1.0f }; }
        pos->x = bestClosest.x + dir.x * allow;
        pos->y = bestClosest.y + dir.y * allow;
    }
}

Vector2 MapBody_GetSafeCenter(void)
{
    return (Vector2){ THORAX_SAFE_X, THORAX_SAFE_Y };
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

// Um lobo pulmonar: várias elipses sobrepostas (lobos) + contorno orgânico.
static void DrawLung(Vector2 hilum, int side, Color col, float glow, float breathe)
{
    // side: -1 esquerdo (do mapa), +1 direito. O pulmão se abre para fora/baixo.
    float w = 210.0f + breathe * 14.0f;
    float h = 300.0f + breathe * 20.0f;
    Vector2 c = { hilum.x + side * 150.0f, hilum.y + 70.0f };
    // brilho de fundo
    DrawCircleV(c, w * 1.25f, Fade(col, glow));
    // lobos (3 elipses verticais ligeiramente deslocadas)
    for (int lobe = 0; lobe < 3; lobe++)
    {
        float ly = c.y - h * 0.32f + lobe * (h * 0.34f);
        float lw = w * (1.0f - lobe * 0.13f);
        float lh = h * 0.42f;
        float lx = c.x + side * lobe * 16.0f;
        DrawEllipse((int)lx, (int)ly, lw, lh, Fade(col, 0.30f));
        DrawEllipseLines((int)lx, (int)ly, lw, lh, Fade(col, 0.85f));
    }
    // contorno externo do pulmão (elipse maior)
    DrawEllipseLines((int)c.x, (int)c.y, w, h, col);
    DrawEllipseLines((int)c.x, (int)c.y, w * 0.98f, h * 0.98f, Fade(col, 0.4f));
    // bronquíolos internos (linhas finas a partir do hilo)
    for (int b = 0; b < 4; b++)
    {
        float t = 0.25f + b * 0.2f;
        Vector2 e = { c.x + side * w * 0.5f * t, c.y - h * 0.3f + b * (h * 0.2f) };
        DrawLineEx(hilum, e, 5.0f, Fade(col, 0.5f));
    }
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
        // ---- PULMÕES (lobos + traqueia + brônquios) ----
        bool fLung = (focus == REGION_LUNGS);
        Color lungCol = fLung ? (Color){ 90, 205, 255, 255 } : (Color){ 120, 150, 180, 205 };
        float lungGlow = fLung ? (0.18f + pulse * 0.22f) : 0.10f;
        // Traqueia (do pescoço até a bifurcação) com "anéis".
        Vector2 trTop = { LUNGS_CX, 1230.0f };
        Vector2 trBif = { LUNGS_CX, 1470.0f };
        DrawLineEx(trTop, trBif, 30.0f, Fade(lungCol, 0.55f));
        for (int k = 0; k < 5; k++)
        {
            float yy = 1250.0f + k * 44.0f;
            DrawLineEx((Vector2){ LUNGS_CX - 16, yy }, (Vector2){ LUNGS_CX + 16, yy }, 4.0f, Fade(lungCol, 0.7f));
        }
        // Brônquios para cada hilo.
        Vector2 hilumL = { LUNGS_CX - LUNG_OFFSET * 0.5f, LUNGS_CY - 40.0f };
        Vector2 hilumR = { LUNGS_CX + LUNG_OFFSET * 0.5f, LUNGS_CY - 40.0f };
        DrawLineEx(trBif, hilumL, 14.0f, Fade(lungCol, 0.6f));
        DrawLineEx(trBif, hilumR, 14.0f, Fade(lungCol, 0.6f));
        DrawLung(hilumL, -1, lungCol, lungGlow, breathe);
        DrawLung(hilumR, +1, lungCol, lungGlow, breathe);

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
