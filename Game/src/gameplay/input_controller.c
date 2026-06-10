#include "../../include/input_controller.h"
#include "../../include/telas.h"
#include "../../include/gameplay.h"
#include "raymath.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

extern UIButton menuButtons[5];
extern UIButton pauseButtons[5];
extern UIButton controlsButton;
extern UIButton gameOverButtons[2];
extern UIButton victoryButtons[2];
extern UIButton settingsButtons[4];
extern UIButton settingsReturnBtn;

extern UIButton settingsBtnVoltar;

void UpdateBtnState(UIButton *btn, Vector2 mouse)
{
    btn->hover = CheckCollisionPointRec(mouse, btn->bounds);
    btn->clicked = false;
    if (btn->hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        btn->clicked = true;
    }
}

bool UpdateButtonsMenu(GameState *game, Vector2 mouse)
{
    // Emite partículas decorativas subindo (verde biológico e azul ciano)
    if (GetRandomValue(0, 8) == 0)
    {
        Vector2 pPos = { (float)GetRandomValue(0, SCREEN_WIDTH), SCREEN_HEIGHT + 10.0f };
        Vector2 pVel = { (float)GetRandomValue(-20, 20), (float)GetRandomValue(-60, -25) };
        Color pCol = (GetRandomValue(0, 1) == 0) ? (Color){0,200,100,255} : (Color){0,229,255,255};
        SpawnParticle(game, pPos, pVel, pCol, (float)GetRandomValue(2, 5), (float)GetRandomValue(3, 6));
    }

    // Atualiza lógica das partículas decorativas
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (game->particles[i].active)
        {
            game->particles[i].position.y += game->particles[i].velocity.y * GetFrameTime();
            game->particles[i].position.x += game->particles[i].velocity.x * GetFrameTime();
            game->particles[i].lifeTime -= GetFrameTime();
            if (game->particles[i].lifeTime <= 0.0f) game->particles[i].active = false;
        }
    }

    // Lógica do Input Box do Nome do Jogador (Y = 200, Altura 40)
    Rectangle nameBounds = { 500, 200, 280, 40 };
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (CheckCollisionPointRec(mouse, nameBounds))
        {
            game->nameInputActive = true;
        }
        else
        {
            game->nameInputActive = false;
        }
    }

    // Digitação se ativo
    if (game->nameInputActive)
    {
        int key = GetCharPressed();
        while (key > 0)
        {
            if ((key >= 32) && (key <= 125) && (strlen(game->player.name) < 15))
            {
                int len = strlen(game->player.name);
                game->player.name[len] = (char)key;
                game->player.name[len + 1] = '\0';
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {
            int len = strlen(game->player.name);
            if (len > 0)
            {
                game->player.name[len - 1] = '\0';
            }
        }

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
        {
            game->nameInputActive = false;
        }
    }

    // Determina se existem arquivos de save
    bool anySaveExists = false;
    for (int i = 1; i <= 3; i++)
    {
        char path[64];
        sprintf(path, "Saves/save_slot_%d.txt", i);
        FILE *fTest = fopen(path, "r");
        if (fTest != NULL)
        {
            fclose(fTest);
            anySaveExists = true;
            break;
        }
    }

    // Evita hover e clique acidentais nos botões do menu durante a digitação
    if (!game->nameInputActive)
    {
        for (int i = 0; i < 5; i++)
        {
            if (i == 1 && !anySaveExists)
            {
                menuButtons[i].hover = false;
                menuButtons[i].clicked = false;
            }
            else
            {
                UpdateBtnState(&menuButtons[i], mouse);
            }
        }
    }
    else
    {
        for (int i = 0; i < 5; i++)
        {
            menuButtons[i].hover = false;
            menuButtons[i].clicked = false;
        }
    }

    // Ações
    if (menuButtons[0].clicked) // NOVO JOGO
    {
        InitGame(game);
        RequestLoadingScreen(game, LOAD_TO_TUTORIAL, 2.0f);
    }
    else if (menuButtons[1].clicked) // CARREGAR JOGO
    {
        // Verifica se algum save existe antes de abrir a tela de slots
        bool anySaveExists = false;
        for (int i = 1; i <= 3; i++)
        {
            char path[64];
            sprintf(path, "Saves/save_slot_%d.txt", i);
            FILE *fTest = fopen(path, "r");
            if (fTest != NULL)
            {
                fclose(fTest);
                anySaveExists = true;
                break;
            }
        }
        
        if (anySaveExists)
        {
            // Carrega os metadados dos slots para a tela de seleção
            for (int i = 0; i < 3; i++)
            {
                game->slotsMeta[i] = CarregarMetadadosSlot(i + 1);
            }
            game->currentScreen = SCREEN_LOAD_SELECT;
        }
    }
    else if (menuButtons[2].clicked) // CONTROLES
    {
        game->currentScreen = SCREEN_CONTROLS;
    }
    else if (menuButtons[3].clicked) // CONFIGURACOES
    {
        game->currentScreen = SCREEN_SETTINGS;
    }

    return menuButtons[4].clicked; // Retorna true se clicou em SAIR
}

void UpdateButtonsControles(GameState *game, Vector2 mouse)
{
    UpdateBtnState(&controlsButton, mouse);
    if (controlsButton.clicked)
    {
        game->currentScreen = SCREEN_MENU;
    }
}

void UpdateButtonsPause(GameState *game, Vector2 mouse)
{
    for (int i = 0; i < 5; i++)
    {
        UpdateBtnState(&pauseButtons[i], mouse);
    }

    if (pauseButtons[0].clicked) // VOLTAR
    {
        game->currentScreen = game->inTutorial ? SCREEN_TUTORIAL : SCREEN_GAMEPLAY;
    }
    else if (pauseButtons[1].clicked) // SALVAR
    {
        // Carrega metadados dos slots para a tela de salvamento
        for (int j = 0; j < 3; j++)
        {
            game->slotsMeta[j] = CarregarMetadadosSlot(j + 1);
        }
        game->currentScreen = SCREEN_SAVE_SELECT;
    }
    else if (pauseButtons[2].clicked) // CARREGAR
    {
        // Carrega metadados dos slots para a tela de carregamento
        for (int j = 0; j < 3; j++)
        {
            game->slotsMeta[j] = CarregarMetadadosSlot(j + 1);
        }
        game->currentScreen = SCREEN_LOAD_SELECT;
    }
    else if (pauseButtons[3].clicked) // CONFIGURACOES
    {
        game->currentScreen = SCREEN_SETTINGS;
    }
    else if (pauseButtons[4].clicked) // MENU
    {
        RequestLoadingScreen(game, LOAD_TO_MENU, 1.5f);
    }
}

void UpdateButtonsGameOver(GameState *game, Vector2 mouse)
{
    for (int i = 0; i < 2; i++)
    {
        UpdateBtnState(&gameOverButtons[i], mouse);
    }

    if (gameOverButtons[0].clicked) // TENTAR NOVAMENTE
    {
        InitGame(game);
        RequestLoadingScreen(game, LOAD_TO_GAMEPLAY, 2.0f);
    }
    else if (gameOverButtons[1].clicked) // MENU
    {
        RequestLoadingScreen(game, LOAD_TO_MENU, 1.5f);
    }
}

void UpdateButtonsVitoria(GameState *game, Vector2 mouse)
{
    // Partículas douradas festivas
    if (GetRandomValue(0, 5) == 0)
    {
        Vector2 pPos = { (float)GetRandomValue(0, SCREEN_WIDTH), SCREEN_HEIGHT + 10.0f };
        Vector2 pVel = { (float)GetRandomValue(-20, 20), (float)GetRandomValue(-80, -35) };
        SpawnParticle(game, pPos, pVel, GOLD, (float)GetRandomValue(2, 5), (float)GetRandomValue(2.5f, 5));
    }

    // Atualiza lógica das partículas festivas
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (game->particles[i].active)
        {
            game->particles[i].position.y += game->particles[i].velocity.y * GetFrameTime();
            game->particles[i].position.x += game->particles[i].velocity.x * GetFrameTime();
            game->particles[i].lifeTime -= GetFrameTime();
            if (game->particles[i].lifeTime <= 0.0f) game->particles[i].active = false;
        }
    }

    for (int i = 0; i < 2; i++)
    {
        UpdateBtnState(&victoryButtons[i], mouse);
    }

    if (victoryButtons[0].clicked) // JOGAR DE NOVO
    {
        InitGame(game);
        RequestLoadingScreen(game, LOAD_TO_GAMEPLAY, 2.0f);
    }
    else if (victoryButtons[1].clicked) // MENU
    {
        RequestLoadingScreen(game, LOAD_TO_MENU, 1.5f);
    }
}



int UpdateButtonsSaveSelect(GameState *game, Vector2 mouse, Texture2D slotTextures[3], bool slotTexturesLoaded[3])
{
    // Verifica cliques nos Cards
    for (int i = 0; i < 3; i++)
    {
        float cardX = 80.0f + (float)i * 400.0f;
        Rectangle cardBounds = { cardX, 140, 320, 430 };

        // Verifica primeiro clique no botão de Apagar se o slot existe
        if (game->slotsMeta[i].exists)
        {
            Rectangle deleteBounds = { cardX + 20, 495, 280, 35 };
            if (CheckCollisionPointRec(mouse, deleteBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                char pathTxt[64];
                char pathPng[64];
                sprintf(pathTxt, "Saves/save_slot_%d.txt", i + 1);
                sprintf(pathPng, "Saves/screenshot_slot_%d.png", i + 1);
                remove(pathTxt);
                remove(pathPng);

                if (slotTexturesLoaded[i])
                {
                    UnloadTexture(slotTextures[i]);
                    slotTextures[i] = (Texture2D){ 0 };
                    slotTexturesLoaded[i] = false;
                }

                game->slotsMeta[i] = CarregarMetadadosSlot(i + 1);
                return 0; // Atualiza a tela sem sair dela
            }
        }

        if (CheckCollisionPointRec(mouse, cardBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            return i + 1; // Retorna o slot selecionado (1, 2 ou 3)
        }
    }

    // Verifica clique no botão Voltar
    UIButton btnVoltar = { { 490, 600, 300, 50 }, "VOLTAR", false, false };
    if (CheckCollisionPointRec(mouse, btnVoltar.bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        return -1; // Sinaliza que clicou em voltar
    }

    return 0;
}

int UpdateButtonsLoadSelect(GameState *game, Vector2 mouse, Texture2D slotTextures[3], bool slotTexturesLoaded[3])
{
    // Verifica cliques nos Cards (apenas slots que existem)
    for (int i = 0; i < 3; i++)
    {
        float cardX = 80.0f + (float)i * 400.0f;
        Rectangle cardBounds = { cardX, 140, 320, 430 };

        if (game->slotsMeta[i].exists)
        {
            // Verifica primeiro clique no botão de Apagar
            Rectangle deleteBounds = { cardX + 20, 495, 280, 35 };
            if (CheckCollisionPointRec(mouse, deleteBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                char pathTxt[64];
                char pathPng[64];
                sprintf(pathTxt, "Saves/save_slot_%d.txt", i + 1);
                sprintf(pathPng, "Saves/screenshot_slot_%d.png", i + 1);
                remove(pathTxt);
                remove(pathPng);

                if (slotTexturesLoaded[i])
                {
                    UnloadTexture(slotTextures[i]);
                    slotTextures[i] = (Texture2D){ 0 };
                    slotTexturesLoaded[i] = false;
                }

                game->slotsMeta[i] = CarregarMetadadosSlot(i + 1);
                return 0; // Atualiza a tela sem sair dela
            }

            if (CheckCollisionPointRec(mouse, cardBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                return i + 1; // Retorna o slot selecionado (1, 2 ou 3)
            }
        }
    }

    // Verifica clique no botão Voltar
    UIButton btnVoltar = { { 490, 600, 300, 50 }, "VOLTAR", false, false };
    if (CheckCollisionPointRec(mouse, btnVoltar.bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        return -1; // Sinaliza que clicou em voltar
    }

    return 0;
}

void UpdateButtonsSettings(GameState *game, Vector2 mouse, GameScreen backScreen)
{
    UpdateBtnState(&settingsBtnVoltar, mouse);
    if (settingsBtnVoltar.clicked)
    {
        game->currentScreen = backScreen;
        return;
    }
    
    Rectangle sliderBounds = { 600, 230, 300, 60 };
    if (CheckCollisionPointRec(mouse, sliderBounds) && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        float pct = (mouse.x - 600.0f) / 300.0f;
        if (pct < 0.0f) pct = 0.0f;
        if (pct > 1.0f) pct = 1.0f;
        game->masterVolume = pct;
    }
}

