// telas.c
// Implementação do HUD, minimapa, efeitos visuais e menus interativos.
#include "../../include/telas.h"
#include "../../include/gameplay.h"
#include "raymath.h"
#include "../../Assets/Maps/map_seringa.h"
#include "../../Assets/Maps/map_organismo.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

extern Vector2 g_virtualMouse;

static void DrawSciFiBox(Rectangle r, Color col);

// ============================================================================
// DEFINIÇÃO DOS BOTÕES DAS TELAS (GLOBAIS DA UI)
// ============================================================================
static UIButton menuButtons[] = {
    { { 500, 260, 280, 40 }, "NEW GAME", false, false },
    { { 500, 310, 280, 40 }, "LOAD GAME", false, false },
    { { 500, 360, 280, 40 }, "CONTROLS", false, false },
    { { 500, 410, 280, 40 }, "SETTINGS", false, false },
    { { 500, 460, 280, 40 }, "EXIT", false, false }
};

static UIButton pauseButtons[] = {
    { { 500, 210, 280, 40 }, "RESUME GAME", false, false },
    { { 500, 260, 280, 40 }, "SAVE PROGRESS", false, false },
    { { 500, 310, 280, 40 }, "LOAD PREVIOUS", false, false },
    { { 500, 360, 280, 40 }, "SETTINGS", false, false },
    { { 500, 410, 280, 40 }, "MAIN MENU", false, false }
};

static UIButton controlsButton = { { 490, 580, 300, 50 }, "BACK", false, false };

static UIButton gameOverButtons[] = {
    { { 490, 390, 300, 50 }, "TRY AGAIN", false, false },
    { { 490, 460, 300, 50 }, "MAIN MENU", false, false }
};

static UIButton victoryButtons[] = {
    { { 490, 390, 300, 50 }, "NEW JOURNEY", false, false },
    { { 490, 460, 300, 50 }, "MAIN MENU", false, false }
};

// ============================================================================
// AUXILIAR: DESENHA BOTÃO ESTILIZADO (GLASSMORPHISM + GRADIENTE DE HOVER)
// ============================================================================
void DrawButton(UIButton botao, Font font, bool enabled)
{
    // Cores de base
    Color corFundo = Fade((Color){ 26, 21, 44, 255 }, 0.75f);
    Color corBorda = (Color){ 104, 76, 172, 255 };
    Color corTexto = WHITE;

    if (!enabled)
    {
        corFundo = Fade((Color){ 15, 12, 24, 255 }, 0.5f);
        corBorda = Fade(GRAY, 0.25f);
        corTexto = Fade(GRAY, 0.4f);
    }
    else
    {
        if (botao.hover)
        {
            corFundo = Fade((Color){ 84, 52, 148, 255 }, 0.85f);
            corBorda = (Color){ 0, 229, 255, 255 }; // Cyan brilhante no hover
            corTexto = (Color){ 0, 229, 255, 255 };
        }

        if (botao.clicked)
        {
            corFundo = (Color){ 120, 80, 220, 255 };
            corTexto = WHITE;
        }
    }

    // Desenha fundo e borda arredondados (Premium feel)
    DrawRectangleRounded(botao.bounds, 0.25f, 8, corFundo);
    DrawRectangleRoundedLines(botao.bounds, 0.25f, 8, corBorda);

    // Centralização do texto com fonte personalizada
    int fontSize = 22;
    Vector2 textSize = MeasureTextEx(font, botao.text, (float)fontSize, 1.0f);
    Vector2 textPos = {
        botao.bounds.x + (botao.bounds.width / 2.0f) - (textSize.x / 2.0f),
        botao.bounds.y + (botao.bounds.height / 2.0f) - (textSize.y / 2.0f)
    };

    DrawTextEx(font, botao.text, textPos, (float)fontSize, 1.0f, corTexto);
}

// ============================================================================
// AUXILIAR: HOVER/CLICK GENÉRICO
// ============================================================================
static void UpdateBtnState(UIButton *btn, Vector2 mouse)
{
    btn->hover = CheckCollisionPointRec(mouse, btn->bounds);
    btn->clicked = false;
    if (btn->hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        btn->clicked = true;
    }
}

