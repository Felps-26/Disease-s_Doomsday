// rpg.c
// Ponto de entrada principal do RPG modular.
#include "../include/game.h"
#include "../include/gameplay.h"
#include "../include/telas.h"
#include "../include/input_controller.h"
#include "../include/asset_manager.h"
#include "logic/fsm.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Globais para escalonamento da janela física e virtual (Letterbox)
float g_scale = 1.0f;
Vector2 g_mouseOffset = { 0.0f, 0.0f };
Vector2 g_virtualMouse = { 0.0f, 0.0f };
Font g_gameFont;

// FSM state variables exported to fsm.c
Texture2D slotTextures[3] = { 0 };
bool slotTexturesLoaded[3] = { false };
GameScreen loadSelectBackScreen = SCREEN_MENU;
GameScreen settingsBackScreen = SCREEN_MENU;
Image screenshotTemp = { 0 };
bool hasScreenshotTemp = false;

int main(void)
{
    // Habilita janela redimensionável e sincronização vertical (VSync)
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Disease's Doomsday");
    SetWindowMinSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    SetTargetFPS(60);
    SetExitKey(0); // Desativa ESC fechar direto para podermos usar como pause/voltar

    // Carrega todos os assets globais (Fonte, Música, SFX)
    LoadGameAssets();
    g_gameFont = g_assets.font;

    // Inicializa o estado global do jogo
    GameState game;
    memset(&game, 0, sizeof(GameState));
    game.currentScreen = SCREEN_MENU;
    game.masterVolume = 1.0f; // Default volume
    game.difficulty = DIFFICULTY_MEDIUM; // padrão antes de ler config
    LoadPlayerConfig(&game);  // Restaura volume, skins e dificuldade (Saves/config.txt)
    SetMasterVolume(game.masterVolume);
    
    GameScreen previousScreen = SCREEN_MENU;

    // Inicialização do Áudio e Carregamento do Tema (DeepVoid.mp3)
    // AudioDevice é iniciado no asset_manager
    Music musicA = g_assets.musicMain;
    Music musicB = g_assets.musicB;
    bool streamAPlaying = true;
    bool crossfadeActive = false;
    bool musicLoaded = (musicA.frameCount > 0) && (musicB.frameCount > 0);

    if (musicLoaded)
    {
        musicA.looping = false;
        musicB.looping = false;
        PlayMusicStream(musicA);
        SetMusicVolume(musicA, 1.0f);
        SetMusicVolume(musicB, 0.0f);
    }

    // Render target para o escalonamento adaptativo (Letterbox virtual)
    RenderTexture2D target = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    // Loop de jogo principal
    while (!WindowShouldClose() && !game.shouldClose)
    {
        // Atualiza a música de fundo com Crossfade de 12 segundos
        if (musicLoaded)
        {
            if (streamAPlaying)
            {
                UpdateMusicStream(musicA);
                float time = GetMusicTimePlayed(musicA);
                float length = GetMusicTimeLength(musicA);
                
                if (length > 12.0f && time >= (length - 12.0f))
                {
                    if (!crossfadeActive)
                    {
                        PlayMusicStream(musicB);
                        crossfadeActive = true;
                    }
                    UpdateMusicStream(musicB);
                    float fadeProgress = (time - (length - 12.0f)) / 12.0f;
                    if (fadeProgress > 1.0f) fadeProgress = 1.0f;
                    if (fadeProgress < 0.0f) fadeProgress = 0.0f;
                    
                    SetMusicVolume(musicA, 1.0f - fadeProgress);
                    SetMusicVolume(musicB, fadeProgress);
                }
                else
                {
                    SetMusicVolume(musicA, 1.0f);
                    SetMusicVolume(musicB, 0.0f);
                    crossfadeActive = false;
                }
                
                if (time >= length - 0.1f || !IsMusicStreamPlaying(musicA))
                {
                    StopMusicStream(musicA);
                    streamAPlaying = false;
                    crossfadeActive = false;
                    SetMusicVolume(musicB, 1.0f);
                }
            }
            else
            {
                UpdateMusicStream(musicB);
                float time = GetMusicTimePlayed(musicB);
                float length = GetMusicTimeLength(musicB);
                
                if (length > 12.0f && time >= (length - 12.0f))
                {
                    if (!crossfadeActive)
                    {
                        PlayMusicStream(musicA);
                        crossfadeActive = true;
                    }
                    UpdateMusicStream(musicA);
                    float fadeProgress = (time - (length - 12.0f)) / 12.0f;
                    if (fadeProgress > 1.0f) fadeProgress = 1.0f;
                    if (fadeProgress < 0.0f) fadeProgress = 0.0f;
                    
                    SetMusicVolume(musicB, 1.0f - fadeProgress);
                    SetMusicVolume(musicA, fadeProgress);
                }
                else
                {
                    SetMusicVolume(musicB, 1.0f);
                    SetMusicVolume(musicA, 0.0f);
                    crossfadeActive = false;
                }
                
                if (time >= length - 0.1f || !IsMusicStreamPlaying(musicB))
                {
                    StopMusicStream(musicB);
                    streamAPlaying = true;
                    crossfadeActive = false;
                    SetMusicVolume(musicA, 1.0f);
                }
            }
        }

        // --------------------------------------------------------------------
        // A. CÁLCULO DE ESCALONAMENTO E COORDENADAS VIRTUAIS
        // --------------------------------------------------------------------
        float scaleX = (float)GetScreenWidth() / SCREEN_WIDTH;
        float scaleY = (float)GetScreenHeight() / SCREEN_HEIGHT;
        g_scale = (scaleX < scaleY) ? scaleX : scaleY;
        
        g_mouseOffset.x = (GetScreenWidth() - (SCREEN_WIDTH * g_scale)) * 0.5f;
        g_mouseOffset.y = (GetScreenHeight() - (SCREEN_HEIGHT * g_scale)) * 0.5f;

        Vector2 rawMouse = GetMousePosition();
        g_virtualMouse.x = (rawMouse.x - g_mouseOffset.x) / g_scale;
        g_virtualMouse.y = (rawMouse.y - g_mouseOffset.y) / g_scale;

        // --------------------------------------------------------------------
        // B. ATUALIZAÇÃO DA LÓGICA CONFORME A TELA ATUAL
        // --------------------------------------------------------------------
        UpdateStateMachine(&game);
        
        // CUIDADO: o main ainda precisa lidar com KEY_F5 (quicksave)
        if (game.currentScreen == SCREEN_GAMEPLAY && IsKeyPressed(KEY_F5))
        {
            // Garante renderização da gameplay limpa antes de capturar
            BeginTextureMode(target);
            ClearBackground(BLACK);
            DrawTelaGameplay(&game, g_gameFont, false);
            EndTextureMode();

            // Quicksave para o Slot 1 (Padrão)
            SalvarJogoSlot(&game, 1);
            
            // Captura e exporta screenshot limpa
            Image quicksaveImg = LoadImageFromTexture(target.texture);
            ImageFlipVertical(&quicksaveImg);
            ImageResize(&quicksaveImg, 280, 158);
            ExportImage(quicksaveImg, "Saves/screenshot_slot_1.png");
            UnloadImage(quicksaveImg);
            
            game.saveLoaded = true;
            strcpy(game.notificationMsg, "GAME SAVED!");
            game.timeElapsed = 0.0f;
        }

        // --------------------------------------------------------------------
        // GESTÃO DE TRANSIÇÕES DE TELA (CAPTURA DE SCREENSHOTS E LOAD DE TEXTURAS)
        // Executado após a atualização para que transições no mesmo frame sejam detectadas
        // --------------------------------------------------------------------
        if (game.currentScreen != previousScreen)
        {
            // Se saiu de uma tela de slots ou de pausa, limpa a screenshot/texturas
            if (previousScreen == SCREEN_SAVE_SELECT || previousScreen == SCREEN_LOAD_SELECT || previousScreen == SCREEN_PAUSE)
            {
                // Limpa texturas dos slots se saiu deles
                if (previousScreen == SCREEN_SAVE_SELECT || previousScreen == SCREEN_LOAD_SELECT)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        if (slotTexturesLoaded[i])
                        {
                            UnloadTexture(slotTextures[i]);
                            slotTextures[i] = (Texture2D){ 0 };
                            slotTexturesLoaded[i] = false;
                        }
                    }
                }
                
                // Limpa o screenshot temporário se não estamos salvando e não estamos na pausa
                if (game.currentScreen != SCREEN_SAVE_SELECT && game.currentScreen != SCREEN_PAUSE)
                {
                    if (hasScreenshotTemp)
                    {
                        UnloadImage(screenshotTemp);
                        hasScreenshotTemp = false;
                    }
                }
            }

            // Entrando na tela de salvamento: garante screenshot e carrega metadados/texturas
            if (game.currentScreen == SCREEN_SAVE_SELECT)
            {
                if (hasScreenshotTemp)
                {
                    UnloadImage(screenshotTemp);
                    hasScreenshotTemp = false;
                }

                // Renderiza a jogabilidade limpa (sem overlays) para a render target
                BeginTextureMode(target);
                ClearBackground(BLACK);
                DrawTelaGameplay(&game, g_gameFont, false);
                EndTextureMode();

                screenshotTemp = LoadImageFromTexture(target.texture);
                ImageFlipVertical(&screenshotTemp);
                ImageResize(&screenshotTemp, 280, 158);
                hasScreenshotTemp = true;

                for (int i = 0; i < 3; i++)
                {
                    char path[64];
                    sprintf(path, "Saves/screenshot_slot_%d.png", i + 1);
                    if (FileExists(path))
                    {
                        slotTextures[i] = LoadTexture(path);
                        slotTexturesLoaded[i] = true;
                    }
                    else
                    {
                        slotTexturesLoaded[i] = false;
                    }
                }

                // Carrega os metadados mais recentes dos slots
                for (int i = 0; i < 3; i++)
                {
                    game.slotsMeta[i] = CarregarMetadadosSlot(i + 1);
                }
            }

            // Entrando na tela de carregamento: carrega metadados/texturas
            if (game.currentScreen == SCREEN_LOAD_SELECT)
            {
                loadSelectBackScreen = previousScreen;

                for (int i = 0; i < 3; i++)
                {
                    char path[64];
                    sprintf(path, "Saves/screenshot_slot_%d.png", i + 1);
                    if (FileExists(path))
                    {
                        slotTextures[i] = LoadTexture(path);
                        slotTexturesLoaded[i] = true;
                    }
                    else
                    {
                        slotTexturesLoaded[i] = false;
                    }
                }

                // Carrega os metadados mais recentes dos slots
                for (int i = 0; i < 3; i++)
                {
                    game.slotsMeta[i] = CarregarMetadadosSlot(i + 1);
                }
            }

            // Entrando na tela de configurações
            if (game.currentScreen == SCREEN_SETTINGS)
            {
                settingsBackScreen = previousScreen;
            }
        }

        previousScreen = game.currentScreen;

        // --------------------------------------------------------------------
        // C. RENDERIZAÇÃO NA TEXTURA VIRTUAL (1280x720) - APENAS MUNDO DO JOGO
        // --------------------------------------------------------------------
        BeginTextureMode(target);
        ClearBackground(BLACK);

        if (game.currentScreen == SCREEN_GAMEPLAY || 
            game.currentScreen == SCREEN_PAUSE || 
            game.currentScreen == SCREEN_SAVE_SELECT ||
            game.currentScreen == SCREEN_TUTORIAL ||
            game.currentScreen == SCREEN_QUIZ ||
            game.currentScreen == SCREEN_UPGRADE ||
            (game.currentScreen == SCREEN_LOAD_SELECT && loadSelectBackScreen == SCREEN_PAUSE))
        {
            // Desenha apenas o mundo 2D (sem o HUD) na textura virtual
            DrawTelaGameplay(&game, g_gameFont, false);
        }

        EndTextureMode();

        // --------------------------------------------------------------------
        // D. DESENHA A TEXTURA REDIMENSIONADA NA TELA FÍSICA
        // --------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        bool useBiologicalShader = g_assets.shaderLoaded && (game.currentScreen == SCREEN_GAMEPLAY || game.currentScreen == SCREEN_TUTORIAL || game.currentScreen == SCREEN_PAUSE);
        
        if (useBiologicalShader)
        {
            float timeVal = (float)GetTime();
            int timeLoc = GetShaderLocation(g_assets.biologicalShader, "time");
            SetShaderValue(g_assets.biologicalShader, timeLoc, &timeVal, SHADER_UNIFORM_FLOAT);
            BeginShaderMode(g_assets.biologicalShader);
        }

        // Renderiza a textura virtual centralizada na tela com letterbox
        DrawTexturePro(
            target.texture,
            (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
            (Rectangle){ g_mouseOffset.x, g_mouseOffset.y, SCREEN_WIDTH * g_scale, SCREEN_HEIGHT * g_scale },
            (Vector2){ 0, 0 }, 
            0.0f, 
            WHITE
        );

        if (useBiologicalShader)
        {
            EndShaderMode();
        }

        // --------------------------------------------------------------------
        // E. DESENHA INTERFACE E HUD COM NITIDEZ NATIVA NA TELA FÍSICA (hudCamera)
        // --------------------------------------------------------------------
        Camera2D hudCamera = { 0 };
        hudCamera.zoom = g_scale;
        hudCamera.offset = g_mouseOffset;
        hudCamera.target = (Vector2){ 0.0f, 0.0f };
        hudCamera.rotation = 0.0f;

        BeginMode2D(hudCamera);

        DrawStateMachine(&game);

        EndMode2D();

        EndDrawing();
    }

    // Limpeza e encerramento de áudio e texturas
    // As músicas são descarregadas no asset_manager

    UnloadRenderTexture(target);
    UnloadGameAssets();
    CloseWindow();

    return 0;
}
