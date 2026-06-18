// map_body.c
// Implementação do Mapa: Corpo Humano (a área jogável É o corpo)
// Disease's Doomsday — Projeto de Saúde Pública / DF
//
// O corpo preenche o mapa (MAP_WIDTH x MAP_HEIGHT) e é a própria arena: o jogo
// acontece DENTRO do corpo. A silhueta é a união de "cápsulas" (segmentos com
// raio) — usadas tanto para desenhar quanto para a COLISÃO que confina o herói
// e os patógenos dentro do corpo.
#include "map_body.h"
#include "../../include/sprite_manager.h"
#include "raylib.h"
#include "raymath.h"
#include <math.h>

// ============================================================================
// GEOMETRIA DO CORPO (coordenadas de mundo, MAP 4000x4000)
// O corpo é grande e centralizado (eixo x = 2000), de pé (cabeça em cima).
// ============================================================================
#define BODY_CX 2000.0f

typedef struct BodyPart { Vector2 a, b; float r; } BodyPart;

// União de cápsulas que forma a silhueta. Partes adjacentes se sobrepõem para
// que o interior seja um espaço contínuo (o herói transita entre elas).
static const BodyPart BODY[] = {
    { { BODY_CX, 560.0f },  { BODY_CX, 560.0f },  430.0f }, // cabeça (círculo)
    { { BODY_CX, 880.0f },  { BODY_CX, 1120.0f }, 200.0f }, // pescoço
    { { BODY_CX, 1180.0f }, { BODY_CX, 2150.0f }, 740.0f }, // tórax
    { { BODY_CX, 2100.0f }, { BODY_CX, 2780.0f }, 600.0f }, // abdome
    { { 1450.0f, 1320.0f }, { 2550.0f, 1320.0f }, 250.0f }, // ombros
    { { 1500.0f, 1360.0f }, { 1120.0f, 2500.0f }, 200.0f }, // braço esquerdo
    { { 2500.0f, 1360.0f }, { 2880.0f, 2500.0f }, 200.0f }, // braço direito
    { { 1650.0f, 2780.0f }, { 2350.0f, 2780.0f }, 340.0f }, // quadril
    { { 1840.0f, 2780.0f }, { 1720.0f, 3760.0f }, 260.0f }, // perna esquerda
    { { 2160.0f, 2780.0f }, { 2280.0f, 3760.0f }, 260.0f }, // perna direita
};
static const int BODY_N = (int)(sizeof(BODY) / sizeof(BODY[0]));

// Órgãos-alvo (centros), agora em escala grande, dentro do tórax/abdome.
#define LUNGS_CX   BODY_CX
#define LUNGS_CY   1560.0f
#define LUNG_OFFSET 340.0f
#define BLOOD_CX   BODY_CX
#define BLOOD_CY   1850.0f
#define HOSP_CX    BODY_CX        // foco da superbactéria: colonização intestinal (abdome)
#define HOSP_CY    2380.0f

// Cores do tecido (interior do corpo)
#define COL_MEMBRANE (Color){ 175, 80, 96, 255 }
#define COL_TISSUE   (Color){ 96, 36, 46, 255 }
#define COL_TISSUE2  (Color){ 120, 46, 58, 255 }

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

static void DrawOrganLabel(Font font, const char *txt, Vector2 pos, Color col)
{
    float fs = 52.0f;
    Vector2 sz = MeasureTextEx(font, txt, fs, 2.0f);
    Vector2 at = { pos.x - sz.x * 0.5f, pos.y - sz.y * 0.5f };
    DrawTextEx(font, txt, (Vector2){ at.x + 2, at.y + 2 }, fs, 2.0f, Fade(BLACK, 0.6f));
    DrawTextEx(font, txt, at, fs, 2.0f, col);
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

Vector2 MapBody_RandomPointInside(Vector2 avoid, float minDistFromAvoid)
{
    for (int tries = 0; tries < 48; tries++)
    {
        Vector2 p = { (float)GetRandomValue(200, MAP_WIDTH - 200),
                      (float)GetRandomValue(200, MAP_HEIGHT - 200) };
        if (MapBody_Contains(p) &&
            Vector2DistanceSqr(p, avoid) >= minDistFromAvoid * minDistFromAvoid)
            return p;
    }
    // Fallback seguro: centro do tórax.
    return (Vector2){ BODY_CX, 1700.0f };
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
        default:                    return (Vector2){ BODY_CX, 1700.0f };
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
// DrawMapBody — desenha o corpo preenchido + órgãos (dentro de BeginMode2D)
// ============================================================================
void DrawMapBody(Font font, int currentWorld, int wave, float time)
{
    // Se houver arte do mapa em PNG, usa-a cobrindo o mundo inteiro.
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
        // 1) Membrana (borda externa): cápsulas um pouco maiores.
        for (int i = 0; i < BODY_N; i++)
            DrawBodyCapsule(BODY[i].a, BODY[i].b, BODY[i].r + 26.0f, COL_MEMBRANE);
        // 2) Tecido interno (preenchimento opaco — sem costuras entre cápsulas).
        for (int i = 0; i < BODY_N; i++)
            DrawBodyCapsule(BODY[i].a, BODY[i].b, BODY[i].r, COL_TISSUE);
        // 3) Textura interna: células/vasos translúcidos espalhados pelo tronco.
        for (int i = 0; i < 70; i++)
        {
            float bx = BODY_CX - 700.0f + fmodf(i * 167.0f, 1400.0f);
            float by = 700.0f + fmodf(i * 233.0f, 2700.0f);
            Vector2 cp = { bx, by };
            if (!MapBody_Contains(cp)) continue;
            float rr = 18.0f + (i % 6) * 10.0f;
            float a = 0.06f + 0.05f * sinf(time * 1.2f + i);
            DrawCircleLines((int)bx, (int)by, rr, Fade(COL_TISSUE2, 0.25f));
            DrawCircleV(cp, rr * 0.5f, Fade(COL_TISSUE2, a));
        }
    }

    BodyRegion focus = MapBody_GetFocusRegion(currentWorld, wave);
    float pulse = 0.5f + 0.5f * sinf(time * 3.0f);

    if (SpriteAvailable(SPR_MAP_ORGANS))
    {
        Vector2 center = { MAP_WIDTH / 2.0f, MAP_HEIGHT / 2.0f };
        DrawSpriteCentered(SPR_MAP_ORGANS, center, (Vector2){ MAP_WIDTH, MAP_HEIGHT }, 0.0f, WHITE);
    }
    else
    {
        // ---- PULMÕES ----
        Color lungCol = (focus == REGION_LUNGS) ? (Color){ 80, 200, 255, 255 } : (Color){ 120, 150, 180, 200 };
        float lungGlow = (focus == REGION_LUNGS) ? (0.22f + pulse * 0.25f) : 0.12f;
        for (int s = -1; s <= 1; s += 2)
        {
            Vector2 lc = { LUNGS_CX + s * LUNG_OFFSET, LUNGS_CY };
            DrawCircleV(lc, 300.0f, Fade(lungCol, lungGlow));
            DrawEllipse((int)lc.x, (int)lc.y, 230.0f, 300.0f, Fade(lungCol, 0.30f));
            DrawEllipseLines((int)lc.x, (int)lc.y, 230.0f, 300.0f, lungCol);
            DrawLineEx((Vector2){ LUNGS_CX, LUNGS_CY - 180 }, lc, 12.0f, Fade(lungCol, 0.6f));
        }
        DrawLineEx((Vector2){ LUNGS_CX, 1180.0f }, (Vector2){ LUNGS_CX, LUNGS_CY - 180 }, 24.0f, Fade(lungCol, 0.6f));

        // ---- CORAÇÃO / CORRENTE SANGUÍNEA ----
        Color bloodCol = (focus == REGION_BLOODSTREAM) ? (Color){ 255, 70, 90, 255 } : (Color){ 200, 70, 80, 200 };
        float bloodGlow = (focus == REGION_BLOODSTREAM) ? (0.24f + pulse * 0.25f) : 0.14f;
        DrawCircleV((Vector2){ BLOOD_CX, BLOOD_CY }, 250.0f, Fade(bloodCol, bloodGlow));
        DrawCircleV((Vector2){ BLOOD_CX, BLOOD_CY }, 130.0f, Fade(bloodCol, 0.5f));
        DrawCircleLines((int)BLOOD_CX, (int)BLOOD_CY, 130.0f, bloodCol);
        DrawLineEx((Vector2){ BLOOD_CX, BLOOD_CY + 130 }, (Vector2){ BODY_CX, 2700.0f }, 14.0f, Fade(bloodCol, 0.5f));

        // ---- FOCO HOSPITALAR (colonização intestinal — abdome) ----
        Color hospCol = (focus == REGION_HOSPITAL_FOCUS) ? (Color){ 255, 210, 60, 255 } : (Color){ 190, 170, 90, 190 };
        float hospGlow = (focus == REGION_HOSPITAL_FOCUS) ? (0.16f + pulse * 0.20f) : 0.10f;
        DrawCircleV((Vector2){ HOSP_CX, HOSP_CY }, 300.0f, Fade(hospCol, hospGlow));
        // alças intestinais estilizadas
        for (int k = 0; k < 4; k++)
        {
            float yy = HOSP_CY - 120.0f + k * 90.0f;
            DrawLineEx((Vector2){ HOSP_CX - 200, yy }, (Vector2){ HOSP_CX + 200, yy }, 26.0f, Fade(hospCol, 0.5f));
        }
        DrawRectangle((int)(HOSP_CX - 24), (int)(HOSP_CY - 84), 48, 168, Fade(hospCol, 0.8f));
        DrawRectangle((int)(HOSP_CX - 84), (int)(HOSP_CY - 24), 168, 48, Fade(hospCol, 0.8f));
    }

    // ---- RÓTULOS DOS ÓRGÃOS ----
    DrawOrganLabel(font, "PULMOES", (Vector2){ LUNGS_CX, LUNGS_CY + 360 },
                   (focus == REGION_LUNGS) ? (Color){ 120, 220, 255, 255 } : Fade(WHITE, 0.5f));
    DrawOrganLabel(font, "CORRENTE SANGUINEA", (Vector2){ BLOOD_CX, BLOOD_CY + 70 },
                   (focus == REGION_BLOODSTREAM) ? (Color){ 255, 120, 130, 255 } : Fade(WHITE, 0.45f));
    DrawOrganLabel(font, "FOCO HOSPITALAR", (Vector2){ HOSP_CX, HOSP_CY + 330 },
                   (focus == REGION_HOSPITAL_FOCUS) ? (Color){ 255, 225, 120, 255 } : Fade(WHITE, 0.5f));
}
