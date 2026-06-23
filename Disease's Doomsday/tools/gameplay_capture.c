// gameplay_capture.c — captura final REAL da gameplay (DrawTelaGameplay): fundo
// corpo.png + inimigos + HUD, exatamente como no jogo. Linka o jogo inteiro
// exceto main.c (define os globais que main.c normalmente fornece).
// Gera: (1) visão geral do corpo inteiro; (2) visão na escala de gameplay.
#include "../include/game.h"
#include "../include/gameplay.h"
#include "../include/telas.h"
#include "../include/asset_manager.h"
#include "../Assets/Maps/map_body.h"
#include <string.h>
#include <stdio.h>

// Globais que src/main.c normalmente define.
float g_scale = 1.0f;
Vector2 g_mouseOffset = { 0.0f, 0.0f };
Vector2 g_virtualMouse = { -100.0f, -100.0f };
Font g_gameFont;
Texture2D slotTextures[3] = { 0 };
bool slotTexturesLoaded[3] = { false };
GameScreen loadSelectBackScreen = SCREEN_MENU;
GameScreen settingsBackScreen = SCREEN_MENU;
Image screenshotTemp = { 0 };
bool hasScreenshotTemp = false;

static void shot(RenderTexture2D rt, const char *name)
{
    Image img = LoadImageFromTexture(rt.texture);
    ImageFlipVertical(&img);
    char path[160]; snprintf(path, sizeof(path), "tools/%s.png", name);
    ExportImage(img, path); UnloadImage(img);
    printf("[gameplay_capture] tools/%s.png\n", name);
}

int main(void)
{
    ChangeDirectory(GetApplicationDirectory());
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "gameplay capture");
    SetTargetFPS(60);
    LoadGameAssets();
    g_gameFont = g_assets.font;

    GameState g; memset(&g, 0, sizeof(g));
    strcpy(g.player.name, "DR. AB");
    g.player.hp = 82; g.player.maxHp = 100; g.player.level = 5; g.player.xp = 40; g.player.xpNeeded = 100;
    g.player.equippedWeapon = 2; g.player.speed = 300; g.musicVolume = 1.0f; g.sfxVolume = 1.0f;
    g.player.squashX = 1.0f; g.player.squashY = 1.0f; g.player.facingDir = 1;
    g.difficulty = DIFFICULTY_MEDIUM; g.screenAnim = 1.0f;
    g.currentScreen = SCREEN_GAMEPLAY;
    g.currentWorld = WORLD_VIRUS;     // Mundo dos Vírus (capsídeo, novos arquétipos)
    g.wave = 3;                       // onda 3: enxame+melee+atirador + elite (mini chefe)
    g.diff.enemyHealthMul = 1.0f; g.diff.summonMul = 1.0f;

    // Jogador no centro seguro do tórax; popula a onda real.
    g.player.position = MapBody_GetSafeCenter();
    for (int i = 0; i < 10; i++) g.player.trail[i] = g.player.position;
    StartNextWave(&g);
    for (int i = 0; i < MAX_ENEMIES; i++)
        if (g.enemies[i].active) { g.enemies[i].spawnAnim = 1.0f; g.enemies[i].animTime = (float)(i % 8) * 0.3f; }

    g.camera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };

    // (1) VISÃO GERAL: corpo inteiro (zoom de enquadramento) + HUD.
    RenderTexture2D rt = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    g.camera.target = (Vector2){ MAP_WIDTH / 2.0f, MAP_HEIGHT / 2.0f };
    g.camera.zoom = (float)SCREEN_HEIGHT / (float)MAP_HEIGHT; // corpo inteiro na altura
    BeginTextureMode(rt); ClearBackground(BLACK);
    DrawTelaGameplay(&g, g_gameFont, true);
    EndTextureMode();
    shot(rt, "gameplay_overview");

    // (2) ESCALA DE GAMEPLAY: câmera seguindo o jogador (zoom 1.0) + HUD.
    g.camera.target = g.player.position;
    g.camera.zoom = 1.0f;
    BeginTextureMode(rt); ClearBackground(BLACK);
    DrawTelaGameplay(&g, g_gameFont, true);
    EndTextureMode();
    shot(rt, "gameplay_zoom");

    // (3) OVERLAY DE DEBUG DE COLISÃO (admin + F1): área caminhável/bloqueada
    //     sobre o corpo inteiro — confirma que a colisão segue a nova silhueta.
    g.adminMode = true; g.debugCollision = true;
    g_virtualMouse = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    g.camera.target = (Vector2){ MAP_WIDTH / 2.0f, MAP_HEIGHT / 2.0f };
    g.camera.zoom = (float)SCREEN_HEIGHT / (float)MAP_HEIGHT;
    BeginTextureMode(rt); ClearBackground(BLACK);
    DrawTelaGameplay(&g, g_gameFont, true);
    EndTextureMode();
    shot(rt, "gameplay_debug");

    UnloadRenderTexture(rt);
    UnloadGameplayResources();
    UnloadGameAssets();
    CloseWindow();
    return 0;
}
