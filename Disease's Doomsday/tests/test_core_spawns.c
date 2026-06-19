// test_core_spawns.c
// ACEITAÇÃO (Etapa 2): valida geometricamente, sem janela gráfica, que para
// MUITAS posições de chefe os Núcleos de Infecção nascem dentro do corpo e são
// alcançáveis por melee. Compila contra map_body.c com stubs das funções de
// desenho da raylib (não precisa de X11/OpenGL).
//
// Build & run (a partir da pasta do projeto):
//   gcc -Iinclude -I/path/to/raylib/headers tests/test_core_spawns.c \
//       Assets/Maps/map_body.c -o /tmp/test_cores -lm && /tmp/test_cores
#include "raylib.h"
#include "../Assets/Maps/map_body.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// ---- Stubs das funções da raylib referenciadas por map_body.c (não usadas no
//      teste, pois não chamamos DrawMapBody). Apenas satisfazem o linker. ----
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
// sprite_manager stubs
bool SpriteAvailable(int id) { (void)id; return false; }
void DrawSpriteCentered(int id, Vector2 c, Vector2 s, float r, Color col) { (void)id;(void)c;(void)s;(void)r;(void)col; }

static float dist(Vector2 a, Vector2 b){ return sqrtf((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)); }

// Melee é possível se existe um ponto de pé do herói (disco de raio do player
// dentro do corpo) dentro do alcance do golpe a partir do núcleo.
static int melee_reachable(Vector2 core)
{
    const float rads[4] = { 50.f, 90.f, 130.f, 160.f };
    for (int ri = 0; ri < 4; ri++)
        for (int a = 0; a < 360; a += 20)
        {
            Vector2 p = { core.x + cosf(a*0.01745329f)*rads[ri],
                          core.y + sinf(a*0.01745329f)*rads[ri] };
            if (MapBody_ContainsWithMargin(p, BODY_PLAYER_RADIUS) && dist(p, core) <= MELEE_REACH)
                return 1;
        }
    return MapBody_ContainsWithMargin(core, BODY_PLAYER_RADIUS);
}

int main(void)
{
    srand(12345);
    const int TRIALS = 20000;
    int outMargin = 0, unreachable = 0, bossOverlap = 0, interOverlap = 0, totalCores = 0;

    for (int t = 0; t < TRIALS; t++)
    {
        // posição de chefe aleatória DENTRO do corpo
        Vector2 boss;
        do {
            boss.x = (float)GetRandomValue(150, 3850);
            boss.y = (float)GetRandomValue(150, 3900);
        } while (!MapBody_Contains(boss));

        Vector2 pts[MAX_CORES];
        int n = MapBody_PlaceCores(boss, pts, MAX_CORES,
                                   CORE_SPAWN_MARGIN, CORE_BOSS_CLEARANCE, CORE_INTER_DISTANCE);
        for (int i = 0; i < n; i++)
        {
            totalCores++;
            // 1) dentro do corpo com margem suficiente para aproximação melee
            if (!MapBody_ContainsWithMargin(pts[i], BODY_PLAYER_RADIUS)) outMargin++;
            // 2) alcançável por melee
            if (!melee_reachable(pts[i])) unreachable++;
            // 3) não sobrepõe o chefe
            if (dist(pts[i], boss) < CORE_BOSS_CLEARANCE - 1.0f) bossOverlap++;
            // 4) não sobrepõe outros núcleos
            for (int j = 0; j < i; j++)
                if (dist(pts[i], pts[j]) < CORE_INTER_DISTANCE - 1.0f) { interOverlap++; break; }
        }
    }

    printf("=== TESTE DE SPAWN DE NUCLEOS (Etapa 2) ===\n");
    printf("Chefes simulados: %d | nucleos validados: %d\n", TRIALS, totalCores);
    printf("Fora da margem do corpo : %d\n", outMargin);
    printf("Inalcancaveis por melee : %d\n", unreachable);
    printf("Sobrepostos ao chefe    : %d\n", bossOverlap);
    printf("Sobrepostos entre si    : %d\n", interOverlap);

    int fail = outMargin + unreachable + bossOverlap + interOverlap;
    printf("%s\n", fail == 0 ? "RESULTADO: PASSOU (todos os nucleos validos e alcancaveis por melee)"
                             : "RESULTADO: FALHOU");
    return fail == 0 ? 0 : 1;
}
