// scene_preview.c — cena estilo gameplay: corpo anatômico (DrawMapBody) com o
// elenco viral por cima (DrawEnemyModel + anel de capsídeo), para conferir
// legibilidade (órgãos atrás, inimigos à frente) e integração. Linka map_body.c
// + enemy_model.c + enemy.c + raylib; stubs forçam o desenho procedural.
#include "raylib.h"
#include "../include/game.h"
#include "../Assets/Maps/map_body.h"
#include "../Assets/@models/enemy_model.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

bool SpriteAvailable(int id) { (void)id; return false; }
void DrawSpriteCentered(int id, Vector2 c, Vector2 s, float r, Color col) { (void)id;(void)c;(void)s;(void)r;(void)col; }

static Font gFont;

static void DrawSceneEnemy(int type, int wave, Vector2 pos, int hpPct)
{
    Enemy e; memset(&e, 0, sizeof(e));
    EnemyInitFromArchetype(&e, type, wave, 1.0f);
    e.active = true; e.state = IDLE; e.spawnAnim = 1.0f; e.animTime = 0.7f; e.position = pos;
    if (hpPct >= 0) e.hp = e.maxHp * hpPct / 100;
    const EnemyArchetype *a = EnemyArchetypeFor(type);
    float tierBase = (e.tier == TIER_3_BOSS) ? 140.0f : (e.tier == TIER_MINIBOSS) ? 90.0f : 45.0f;
    float size = tierBase * (a ? a->sizeScale : 1.0f);
    DrawEnemyModel(&e, pos, size, 0.0f, 1.0f, 1.0f);
    if (e.shieldActive && e.shieldHp > 0)
    {
        float shR = size * 1.6f;
        Color sc = (a && a->behavior == BEHAV_ELITE) ? (Color){ 190, 140, 255, 255 }
                 : (a && a->behavior == BEHAV_SWARM) ? (Color){ 150, 220, 235, 255 }
                                                     : (Color){ 120, 200, 255, 255 };
        DrawCircleLines((int)pos.x, (int)pos.y, shR, Fade(sc, 0.7f));
        DrawCircleLines((int)pos.x, (int)pos.y, shR * 0.92f, Fade(sc, 0.3f));
    }
}

int main(void)
{
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(64, 64, "scene preview");
    gFont = GetFontDefault();

    const int PX = 1100;
    RenderTexture2D rt = LoadRenderTexture(PX, PX);
    Camera2D cam = { 0 };
    cam.target = (Vector2){ 2000, 1850 };       // tronco/abdome
    cam.offset = (Vector2){ PX / 2.0f, PX / 2.0f };
    cam.zoom   = (float)PX / 2600.0f;           // mostra a maior parte do corpo

    BeginTextureMode(rt);
    ClearBackground((Color){ 7, 4, 8, 255 });
    BeginMode2D(cam);

    // Camada anatômica (foco nos vírus -> onda 2, Mundo dos Vírus).
    DrawMapBody(gFont, WORLD_VIRUS, 2, 0.8f);

    // Elenco viral por cima dos órgãos (como no gameplay).
    // Enxame (vários, perto) + melee + atirador + elite + chefe.
    for (int i = 0; i < 5; i++)
    {
        float a = i * (2.0f * PI / 5.0f);
        Vector2 p = { 1780.0f + cosf(a) * 120.0f, 1500.0f + sinf(a) * 90.0f };
        DrawSceneEnemy(ETYPE_VIRUS_SWARM, 3, p, -1);
    }
    DrawSceneEnemy(ETYPE_VIRUS_MELEE,  3, (Vector2){ 2230, 1520 }, -1);
    DrawSceneEnemy(ETYPE_VIRUS_RANGED, 3, (Vector2){ 1700, 2050 }, -1);
    DrawSceneEnemy(ETYPE_VIRUS_ELITE,  3, (Vector2){ 2300, 2050 }, -1);
    DrawSceneEnemy(ETYPE_VIRUS_BOSS,   5, (Vector2){ 2000, 2300 }, 20); // chefe com capsídeo roto

    EndMode2D();
    EndTextureMode();

    Image img = LoadImageFromTexture(rt.texture);
    ImageFlipVertical(&img);
    ExportImage(img, "tools/scene_virus.png");
    UnloadImage(img);
    UnloadRenderTexture(rt);
    printf("[scene_preview] tools/scene_virus.png\n");
    CloseWindow();
    return 0;
}
