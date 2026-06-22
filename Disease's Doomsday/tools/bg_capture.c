// bg_capture.c — captura final do mapa usando a TEXTURA real (corpo.png via
// SPR_MAP_BODY) através de DrawMapBody(). Linka o sistema real de sprites, então
// SpriteAvailable(SPR_MAP_BODY) é verdadeiro e exercita o caminho oficial.
// Gera (1) overlay de alinhamento (pontos da área caminhável sobre a imagem) e
// (2) cena com jogador/inimigos por cima do fundo.
//
// Build (a partir da pasta do projeto, binário fica aqui p/ resolver Assets/):
//   cc -O2 -Iinclude -I/opt/homebrew/include tools/bg_capture.c \
//      Assets/Maps/map_body.c Assets/@models/enemy_model.c src/entities/enemy.c \
//      src/assets/sprite_manager.c -o bg_capture -L/opt/homebrew/lib -lraylib \
//      -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio \
//      -framework CoreVideo -lm && ./bg_capture
#include "raylib.h"
#include "../include/game.h"
#include "../include/sprite_manager.h"
#include "../Assets/Maps/map_body.h"
#include "../Assets/@models/enemy_model.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

static Font gFont;
static const int PX = 1100;

static Camera2D worldCam(void)
{
    Camera2D cam = { 0 };
    cam.target = (Vector2){ MAP_WIDTH / 2.0f, MAP_HEIGHT / 2.0f };
    cam.offset = (Vector2){ PX / 2.0f, PX / 2.0f };
    cam.zoom   = (float)PX / (float)MAP_WIDTH; // corpo inteiro cabe no quadro
    return cam;
}

static void shot(RenderTexture2D rt, const char *name)
{
    Image img = LoadImageFromTexture(rt.texture);
    ImageFlipVertical(&img);
    char path[160]; snprintf(path, sizeof(path), "tools/%s.png", name);
    ExportImage(img, path); UnloadImage(img);
    printf("[bg_capture] tools/%s.png\n", name);
}

static void enemyAt(int type, int wave, Vector2 p, int hpPct)
{
    Enemy e; memset(&e, 0, sizeof(e));
    EnemyInitFromArchetype(&e, type, wave, 1.0f);
    e.active = true; e.state = IDLE; e.spawnAnim = 1.0f; e.animTime = 0.6f; e.position = p;
    if (hpPct >= 0) e.hp = e.maxHp * hpPct / 100;
    const EnemyArchetype *a = EnemyArchetypeFor(type);
    float tierBase = (e.tier == TIER_3_BOSS) ? 140.0f : (e.tier == TIER_MINIBOSS) ? 90.0f : 45.0f;
    DrawEnemyModel(&e, p, tierBase * (a ? a->sizeScale : 1.0f), 0.0f, 1.0f, 1.0f);
}

int main(void)
{
    ChangeDirectory(GetApplicationDirectory());
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(64, 64, "bg capture");
    LoadSprites();
    gFont = GetFontDefault();
    printf("corpo.png carregado: %s\n", SpriteAvailable(SPR_MAP_BODY) ? "SIM" : "NAO");

    Camera2D cam = worldCam();

    // (1) Overlay de alinhamento: imagem + pontos da AREA CAMINHAVEL + bbox.
    RenderTexture2D rt = LoadRenderTexture(PX, PX);
    BeginTextureMode(rt);
    ClearBackground((Color){ 7, 4, 8, 255 });
    BeginMode2D(cam);
    DrawMapBody(gFont, WORLD_VIRUS, 2, 0.8f);
    // pontos verdes onde o disco do jogador cabe (area caminhavel real da mascara)
    for (int y = 120; y < MAP_HEIGHT; y += 90)
        for (int x = 120; x < MAP_WIDTH; x += 90)
        {
            Vector2 p = { (float)x, (float)y };
            if (MapBody_ContainsWithMargin(p, BODY_PLAYER_RADIUS))
                DrawCircleV(p, 9.0f, Fade((Color){ 80, 255, 120, 255 }, 0.5f));
        }
    Rectangle b = MapBody_WorldBounds();
    DrawRectangleLinesEx(b, 8.0f, Fade((Color){ 0, 229, 255, 255 }, 0.8f));
    EndMode2D();
    EndTextureMode();
    shot(rt, "bg_align");

    // (2) Cena: imagem + jogador + inimigos por cima (sem overlay).
    BeginTextureMode(rt);
    ClearBackground((Color){ 7, 4, 8, 255 });
    BeginMode2D(cam);
    DrawMapBody(gFont, WORLD_VIRUS, 2, 0.8f);
    enemyAt(ETYPE_VIRUS_SWARM, 3, (Vector2){ 1820, 1500 }, -1);
    enemyAt(ETYPE_VIRUS_SWARM, 3, (Vector2){ 2180, 1520 }, -1);
    enemyAt(ETYPE_VIRUS_MELEE, 3, (Vector2){ 1750, 2050 }, -1);
    enemyAt(ETYPE_VIRUS_RANGED, 3, (Vector2){ 2250, 2050 }, -1);
    enemyAt(ETYPE_VIRUS_ELITE, 3, (Vector2){ 2300, 2450 }, -1);
    enemyAt(ETYPE_VIRUS_BOSS, 5, (Vector2){ 2000, 2750 }, 20);
    // jogador (marcador simples) no centro seguro
    Vector2 pl = MapBody_GetSafeCenter();
    DrawCircleV(pl, 40.0f, (Color){ 0, 229, 255, 255 });
    DrawCircleLines((int)pl.x, (int)pl.y, 44.0f, WHITE);
    EndMode2D();
    EndTextureMode();
    shot(rt, "bg_scene");

    UnloadRenderTexture(rt);
    UnloadSprites();
    CloseWindow();
    return 0;
}
