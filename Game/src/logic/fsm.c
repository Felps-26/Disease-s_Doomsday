#include "fsm.h"
#include "../../include/gameplay.h"
#include "../../include/telas.h"
#include "../../include/input_controller.h"
#include <stdio.h>
#include <string.h>

extern Vector2 g_virtualMouse;
extern GameScreen loadSelectBackScreen;
extern GameScreen settingsBackScreen;
extern Texture2D slotTextures[3];
extern bool slotTexturesLoaded[3];
extern Image screenshotTemp;
extern bool hasScreenshotTemp;

void UpdateStateMachine(GameState *game)
{
    float delta = GetFrameTime();

    switch (game->currentScreen)
    {
        case SCREEN_MENU:
            if (UpdateButtonsMenu(game, g_virtualMouse))
            {
                game->shouldClose = true;
            }
            break;

        case SCREEN_TUTORIAL:
            if (IsKeyPressed(KEY_ESCAPE))
            {
                game->currentScreen = SCREEN_PAUSE;
                game->uiAnimTimer = 0.0f;
            }
            else
            {
                UpdateTutorial(game, delta);
            }
            break;

        case SCREEN_LOADING:
            UpdateTelaLoading(game, delta);
            break;

        case SCREEN_QUIZ:
            UpdateTelaQuiz(game, g_virtualMouse);
            break;

        case SCREEN_UPGRADE:
            UpdateTelaUpgrade(game, g_virtualMouse);
            break;

        case SCREEN_CONTROLS:
            UpdateTelaTutorial(game, g_virtualMouse);
            break;

        case SCREEN_ARSENAL:
            UpdateTelaArsenal(game, g_virtualMouse);
            break;

        case SCREEN_SKINS:
            UpdateTelaSkins(game, g_virtualMouse);
            break;

        case SCREEN_ADMIN:
            UpdateTelaAdmin(game, g_virtualMouse);
            break;

        case SCREEN_SETTINGS:
            UpdateButtonsSettings(game, g_virtualMouse, settingsBackScreen);
            SetMasterVolume(game->masterVolume);
            break;

        case SCREEN_GAMEPLAY:
            if (IsKeyPressed(KEY_ESCAPE))
            {
                game->currentScreen = SCREEN_PAUSE;
                game->uiAnimTimer = 0.0f;
            }
            else if (IsKeyPressed(KEY_F5))
            {
                // Treated in main.c directly for now to handle quicksave easily
            }
            else if (IsKeyPressed(KEY_F9))
            {
                if (FileExists("Saves/save_slot_1.txt"))
                {
                    game->loadSlot = 1;
                    RequestLoadingScreen(game, LOAD_TO_GAMEPLAY, 2.0f);
                }
            }
            else
            {
                UpdateGameplay(game, delta);
            }
            break;

        case SCREEN_PAUSE:
            game->uiAnimTimer += delta;
            if (IsKeyPressed(KEY_ESCAPE))
            {
                game->currentScreen = game->inTutorial ? SCREEN_TUTORIAL : SCREEN_GAMEPLAY;
            }
            else
            {
                UpdateButtonsPause(game, g_virtualMouse);
            }
            break;

        case SCREEN_SAVE_SELECT:
            {
                int slotSelected = UpdateButtonsSaveSelect(game, g_virtualMouse, slotTextures, slotTexturesLoaded);
                if (slotSelected > 0)
                {
                    SalvarJogoSlot(game, slotSelected);
                    if (hasScreenshotTemp)
                    {
                        char path[64];
                        sprintf(path, "Saves/screenshot_slot_%d.png", slotSelected);
                        ExportImage(screenshotTemp, path);
                    }
                    game->currentScreen = SCREEN_GAMEPLAY;
                    game->saveLoaded = true;
                    strcpy(game->notificationMsg, "GAME SAVED!");
                    game->timeElapsed = 0.0f;
                }
                else if (slotSelected == -1)
                {
                    game->currentScreen = SCREEN_PAUSE;
                }
            }
            break;

        case SCREEN_LOAD_SELECT:
            {
                int slotSelected = UpdateButtonsLoadSelect(game, g_virtualMouse, slotTextures, slotTexturesLoaded);
                if (slotSelected > 0)
                {
                    game->loadSlot = slotSelected;
                    RequestLoadingScreen(game, LOAD_TO_GAMEPLAY, 2.0f);
                }
                else if (slotSelected == -1)
                {
                    game->currentScreen = loadSelectBackScreen;
                }
            }
            break;

        case SCREEN_GAMEOVER:
            UpdateButtonsGameOver(game, g_virtualMouse);
            break;

        case SCREEN_VICTORY:
            UpdateButtonsVitoria(game, g_virtualMouse);
            break;

        case SCREEN_WORLD_TRANSITION:
            UpdateTelaTransicao(game);
            break;
    }
}

void DrawStateMachine(GameState *game)
{
    extern Font g_gameFont;
    switch (game->currentScreen)
    {
        case SCREEN_MENU:
            DrawTelaMenu(game, g_gameFont, (float)GetTime());
            break;

        case SCREEN_TUTORIAL:
            DrawTelaTutorial(game, g_gameFont);
            break;

        case SCREEN_LOADING:
            DrawTelaLoading(game, g_gameFont);
            break;

        case SCREEN_QUIZ:
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.8f));
            DrawTelaQuiz(game, g_gameFont);
            break;

        case SCREEN_UPGRADE:
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.8f));
            DrawTelaUpgrade(game, g_gameFont);
            break;

        case SCREEN_CONTROLS:
            DrawTelaControles(game, g_gameFont);
            break;

        case SCREEN_ARSENAL:
            DrawTelaArsenal(game, g_gameFont);
            break;

        case SCREEN_SKINS:
            DrawTelaSkins(game, g_gameFont);
            break;

        case SCREEN_ADMIN:
            DrawTelaAdmin(game, g_gameFont);
            break;

        case SCREEN_SETTINGS:
            DrawTelaSettings(game, g_gameFont);
            break;

        case SCREEN_GAMEPLAY:
            DrawHUD(game, g_gameFont);
            break;

        case SCREEN_PAUSE:
            DrawTelaPausa(game, g_gameFont);
            break;

        case SCREEN_SAVE_SELECT:
            DrawTelaSaveSelect(game, g_gameFont, g_virtualMouse, slotTextures, slotTexturesLoaded);
            break;

        case SCREEN_LOAD_SELECT:
            DrawTelaLoadSelect(game, g_gameFont, g_virtualMouse, slotTextures, slotTexturesLoaded);
            break;

        case SCREEN_GAMEOVER:
            DrawTelaGameOver(game, g_gameFont);
            break;

        case SCREEN_VICTORY:
            DrawTelaVitoria(game, g_gameFont);
            break;

        case SCREEN_WORLD_TRANSITION:
            DrawTelaTransicao(game, g_gameFont);
            break;

        default: break;
    }

    // ------------------------------------------------------------------------
    // TRANSIÇÃO SUAVE (fade-in) ao trocar de tela. Não aplica em GAMEPLAY/PAUSE
    // para não cobrir o combate. Dá um acabamento mais profissional aos menus.
    // ------------------------------------------------------------------------
    static GameScreen prevScreen = SCREEN_MENU;
    static float fadeTimer = 0.0f;
    if (game->currentScreen != prevScreen)
    {
        prevScreen = game->currentScreen;
        if (game->currentScreen != SCREEN_GAMEPLAY && game->currentScreen != SCREEN_PAUSE)
            fadeTimer = 0.30f;
    }
    if (fadeTimer > 0.0f)
    {
        fadeTimer -= GetFrameTime();
        if (fadeTimer < 0.0f) fadeTimer = 0.0f;
        float a = (fadeTimer / 0.30f) * 0.7f;
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, a));
    }
}
