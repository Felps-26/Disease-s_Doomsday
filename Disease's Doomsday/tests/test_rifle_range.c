// test_rifle_range.c
// ACEITAÇÃO (Fase 5): valida DETERMINISTICAMENTE que nenhum projétil de rifle do
// jogador permanece ativo além do alcance (maxRange) mais a tolerância de UM
// frame, que todo projétil sempre termina (alcance ou void), e que SpawnProjectile
// configura velocidade/alcance/origem corretamente. Usa as funções REAIS do jogo
// (SpawnProjectile + Projectile_Advance) e o grid de colisão real (map_body.c).
//
// Build & run (a partir da pasta do projeto):
//   gcc -Iinclude -I<raylib/headers> tests/test_rifle_range.c \
//       src/entities/projectiles.c Assets/Maps/map_body.c -o /tmp/test_rr -lm && /tmp/test_rr
#include "raylib.h"
#include "../include/game.h"
#include "../src/entities/projectiles.h"
#include "../Assets/Maps/map_body.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// ---- Stubs das funções da raylib referenciadas por map_body.c (desenho) ----
void DrawLineEx(Vector2 a, Vector2 b, float t, Color c) { (void)a;(void)b;(void)t;(void)c; }
void DrawCircleV(Vector2 c, float r, Color col) { (void)c;(void)r;(void)col; }
void DrawCircleLines(int x, int y, float r, Color c) { (void)x;(void)y;(void)r;(void)c; }
void DrawEllipse(int x, int y, float rx, float ry, Color c) { (void)x;(void)y;(void)rx;(void)ry;(void)c; }
void DrawEllipseLines(int x, int y, float rx, float ry, Color c) { (void)x;(void)y;(void)rx;(void)ry;(void)c; }
void DrawTriangle(Vector2 a, Vector2 b, Vector2 cc, Color c) { (void)a;(void)b;(void)cc;(void)c; }
void DrawRectangleRounded(Rectangle r, float a, int s, Color c) { (void)r;(void)a;(void)s;(void)c; }
void DrawRectangleRoundedLines(Rectangle r, float a, int s, Color c) { (void)r;(void)a;(void)s;(void)c; }
void DrawTextEx(Font f, const char *t, Vector2 p, float s, float sp, Color c) { (void)f;(void)t;(void)p;(void)s;(void)sp;(void)c; }
Vector2 MeasureTextEx(Font f, const char *t, float s, float sp) { (void)f;(void)t;(void)s;(void)sp; return (Vector2){100,40}; }
Color Fade(Color c, float a) { (void)a; return c; }
int GetRandomValue(int mn, int mx) { return mn + rand() % (mx - mn + 1); }
bool SpriteAvailable(int id) { (void)id; return false; }
void DrawSpriteCentered(int id, Vector2 c, Vector2 s, float r, Color col) { (void)id;(void)c;(void)s;(void)r;(void)col; }
Texture2D GetSprite(int id) { (void)id; return (Texture2D){0}; }

static float frand(float a, float b){ return a + (b-a)*((float)rand()/(float)RAND_MAX); }
static float dist(Vector2 a, Vector2 b){ return sqrtf((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)); }

int main(void)
{
    srand(7);
    const float DT = 1.0f/60.0f;
    GameState *gs = (GameState*)calloc(1, sizeof(GameState));

    // --- validação de SpawnProjectile (velocidade/alcance/origem) ---
    Vector2 c0 = MapBody_GetSafeCenter();
    SpawnProjectile(gs, c0, (Vector2){ c0.x + 1000.0f, c0.y }, PROJ_PLAYER_PHAGE, 10);
    Projectile *sp = &gs->projectiles[0];
    float speed = sqrtf(sp->velocity.x*sp->velocity.x + sp->velocity.y*sp->velocity.y);
    int spawnBad = 0;
    if (!sp->active) spawnBad++;
    if (fabsf(sp->maxRange - 1050.0f) > 0.5f) spawnBad++;
    if (fabsf(speed - 620.0f) > 1.0f) spawnBad++;
    if (dist(sp->origin, c0) > 0.5f) spawnBad++;
    float oneFrame = speed * DT;
    printf("Spawn: speed=%.1f maxRange=%.0f origemOK=%d  (esperado 620 / 1050)\n",
           speed, sp->maxRange, dist(sp->origin,c0) <= 0.5f);

    // --- varredura determinística de alcance/terminação ---
    const int TRIALS = 6000;
    int neverEnded = 0, beyondWhileActive = 0, beyondAtDeact = 0, tooManySteps = 0, ended = 0;
    float worstActive = 0.0f, worstDeact = 0.0f; int worstSteps = 0;
    int stepBound = (int)ceilf(1050.0f / oneFrame) + 2; // alcance / passo + folga

    for (int t = 0; t < TRIALS; t++)
    {
        // origem dentro do corpo (com folga de jogador) e direção aleatória
        Vector2 o;
        do { o.x = frand(0, MAP_WIDTH); o.y = frand(0, MAP_HEIGHT); } while (!MapBody_ContainsWithMargin(o, BODY_PLAYER_RADIUS));
        float ang = frand(0.0f, 6.2831853f);
        Vector2 target = { o.x + cosf(ang)*3000.0f, o.y + sinf(ang)*3000.0f };

        for (int i = 0; i < MAX_PROJECTILES; i++) gs->projectiles[i].active = false;
        SpawnProjectile(gs, o, target, PROJ_PLAYER_PHAGE, 10);
        Projectile *p = &gs->projectiles[0];

        int steps = 0; bool deact = false; float deactDist = 0.0f;
        while (p->active && steps < 4000)
        {
            bool still = Projectile_Advance(p, DT);
            steps++;
            float d = dist(p->position, p->origin);
            if (still) { if (d > worstActive) worstActive = d;
                         if (d > 1050.0f + 1.0f) beyondWhileActive++; }
            else { deact = true; deactDist = d; break; }
        }
        if (!deact) { neverEnded++; continue; }
        ended++;
        if (deactDist > worstDeact) worstDeact = deactDist;
        if (steps > worstSteps) worstSteps = steps;
        // alcance + 1 frame de tolerância (void pode desativar ANTES, com d menor)
        if (deactDist > 1050.0f + oneFrame + 1.0f) beyondAtDeact++;
        if (steps > stepBound) tooManySteps++;
    }

    printf("Alcance: trials=%d terminaram=%d nuncaTerminou=%d\n", TRIALS, ended, neverEnded);
    printf("  ativos_alem_do_alcance=%d  desativados_alem(+1frame)=%d  passos_excedidos=%d\n",
           beyondWhileActive, beyondAtDeact, tooManySteps);
    printf("  worst: ativoMax=%.0fpx deactMax=%.0fpx passosMax=%d (limite passos=%d, 1frame=%.1fpx)\n",
           worstActive, worstDeact, worstSteps, stepBound, oneFrame);

    int fail = spawnBad + neverEnded + beyondWhileActive + beyondAtDeact + tooManySteps;
    printf("%s\n", fail==0 ? "RESULTADO: PASSOU (alcance limitado, sem limpeza off-screen)"
                           : "RESULTADO: FALHOU");
    free(gs);
    return fail==0 ? 0 : 1;
}
