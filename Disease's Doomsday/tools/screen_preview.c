// screen_preview.c — valida (1) a tela de loading de injeção SEM a cena Y/placas
// e (2) a nitidez do personagem: pipeline ANTIGO (alvo 1280 -> upscale) vs NOVO
// (alvo 2x -> downscale). Linka o jogo inteiro exceto main.c. (Scaffolding.)
#include "../include/game.h"
#include "../include/gameplay.h"
#include "../include/telas.h"
#include "../include/asset_manager.h"
#include "../Assets/@models/player_model.h"
#include "rlgl.h"
#include <string.h>
#include <stdio.h>

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

static void ShotRT(RenderTexture2D rt, const char *name)
{
    Image img = LoadImageFromTexture(rt.texture);
    ImageFlipVertical(&img);
    char path[160]; snprintf(path, sizeof(path), "tools/preview_%s.png", name);
    ExportImage(img, path); UnloadImage(img);
    printf("[screen_preview] %s\n", path);
}

int main(void)
{
    ChangeDirectory(GetApplicationDirectory());
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "screen preview");
    SetTargetFPS(60);
    LoadGameAssets();
    g_gameFont = g_assets.font;

    GameState g; memset(&g, 0, sizeof(g));
    strcpy(g.player.name, "DR. AB");
    g.player.hp = 78; g.player.maxHp = 100; g.player.level = 4; g.player.xpNeeded = 100;
    g.player.equippedWeapon = 1; g.player.speed = 300; g.masterVolume = 1.0f;
    g.player.squashX = 1.0f; g.player.squashY = 1.0f; g.player.facingDir = 1; g.player.isMoving = false;
    g.difficulty = DIFFICULTY_MEDIUM; g.screenAnim = 1.0f;

    // ---- (1) Loading de INJEÇÃO (deve estar limpo: sem Y, sem placas cinzas) ----
    RenderTexture2D ui = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    g.currentScreen = SCREEN_LOADING; g.loadTarget = LOAD_TO_GAMEPLAY;
    g.loadingTimer = 1.0f; g.loadingDuration = 2.0f; g.loadingTip = 0; g.syringeTransitionFX = true;
    BeginTextureMode(ui); ClearBackground(BLACK); DrawTelaLoading(&g, g_gameFont); EndTextureMode();
    ShotRT(ui, "loading_injection_clean");
    UnloadRenderTexture(ui);

    // ---- (2) Comparação de NITIDEZ do personagem na gameplay ----
    // Cena: jogador no centro do corpo, câmera de gameplay (zoom 1).
    g.currentScreen = SCREEN_GAMEPLAY; g.syringeTransitionFX = false; g.wave = 1;
    g.player.position = (Vector2){ 2000.0f, 2000.0f };
    for (int i = 0; i < 10; i++) g.player.trail[i] = g.player.position;
    g.camera.target = g.player.position;
    g.camera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    g.camera.zoom = 1.0f;

    // Janela simulada do usuário: 1512x982 -> letterbox region ~1512x850 (g_scale=1.18)
    const int FW = 1512, FH = 850;

    RenderTexture2D lowRT = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);             // 1280x720 (ANTIGO)
    RenderTexture2D hiRT  = LoadRenderTexture(SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2);     // 2560x1440 (NOVO SSAA)
    RenderTexture2D fin   = LoadRenderTexture(FW, FH);
    SetTextureFilter(lowRT.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(hiRT.texture, TEXTURE_FILTER_BILINEAR);

    // Só o PERSONAGEM (tamanho de gameplay, 60), fundo neutro, para isolar a nitidez.
    Player *pl = &g.player;

    // ANTIGO: personagem no alvo 1280x720
    BeginTextureMode(lowRT); ClearBackground((Color){ 16, 20, 28, 255 });
    BeginMode2D(g.camera);
    DrawPlayerModel(pl, 60.0f, (Color){ 0, 229, 255, 255 }, 0.5f, 0.0f);
    EndMode2D(); EndTextureMode();

    // NOVO: personagem no alvo 2x com override de projeção (espaço lógico 1280x720)
    BeginTextureMode(hiRT); ClearBackground((Color){ 16, 20, 28, 255 });
    rlDrawRenderBatchActive(); rlMatrixMode(RL_PROJECTION); rlPushMatrix(); rlLoadIdentity();
    rlOrtho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, -1.0, 1.0);
    rlMatrixMode(RL_MODELVIEW); rlLoadIdentity();
    BeginMode2D(g.camera);
    DrawPlayerModel(pl, 60.0f, (Color){ 0, 229, 255, 255 }, 0.5f, 0.0f);
    EndMode2D();
    rlDrawRenderBatchActive(); rlMatrixMode(RL_PROJECTION); rlPopMatrix(); rlMatrixMode(RL_MODELVIEW); rlLoadIdentity();
    EndTextureMode();

    // ANTIGO: upscale 1280 -> 1512 (bilinear) = borrado
    BeginTextureMode(fin); ClearBackground(BLACK);
    DrawTexturePro(lowRT.texture, (Rectangle){ 0, 0, SCREEN_WIDTH, -SCREEN_HEIGHT }, (Rectangle){ 0, 0, FW, FH }, (Vector2){ 0, 0 }, 0.0f, WHITE);
    EndTextureMode();
    ShotRT(fin, "crisp_BEFORE_upscale1280");

    // NOVO: downscale 2560 -> 1512 (bilinear) = nítido (SSAA)
    BeginTextureMode(fin); ClearBackground(BLACK);
    DrawTexturePro(hiRT.texture, (Rectangle){ 0, 0, SCREEN_WIDTH * 2, -SCREEN_HEIGHT * 2 }, (Rectangle){ 0, 0, FW, FH }, (Vector2){ 0, 0 }, 0.0f, WHITE);
    EndTextureMode();
    ShotRT(fin, "crisp_AFTER_ssaa2x");

    UnloadRenderTexture(lowRT); UnloadRenderTexture(hiRT); UnloadRenderTexture(fin);
    UnloadGameplayResources(); UnloadGameAssets(); CloseWindow();
    return 0;
}
