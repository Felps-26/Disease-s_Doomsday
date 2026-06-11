// telas.c
// Implementação do HUD, minimapa, efeitos visuais e menus interativos.
#include "../../include/telas.h"
#include "../../include/gameplay.h"
#include "../../include/asset_manager.h"
#include "raymath.h"
#include "../../Assets/Maps/map_seringa.h"
#include "../../Assets/@models/player_model.h"
#include "../../Assets/@models/enemy_model.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "rlgl.h"

extern Vector2 g_virtualMouse;

static float EaseOutCubic(float t) {
    t -= 1.0f;
    return t * t * t + 1.0f;
}

// Smoothstep clássico (para o efeito de compressão na transição)
static float SmoothStep01(float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    return t * t * (3.0f - 2.0f * t);
}

// ============================================================================
// CACHE: EXISTE ALGUM SAVE? (evita fopen em disco a cada frame no menu)
// ============================================================================
bool AnySaveExistsCached(void)
{
    static double lastCheck = -10.0;
    static bool cached = false;

    double now = GetTime();
    if (now - lastCheck > 1.0)
    {
        lastCheck = now;
        cached = false;
        for (int i = 1; i <= 3; i++)
        {
            char path[64];
            sprintf(path, "Saves/save_slot_%d.txt", i);
            if (FileExists(path))
            {
                cached = true;
                break;
            }
        }
    }
    return cached;
}

// ============================================================================
// DEFINIÇÃO DOS BOTÕES DAS TELAS (GLOBAIS DA UI)
// ============================================================================
UIButton menuButtons[] = {
    { { 500, 285, 280, 40 }, "NEW GAME", false, false },
    { { 500, 335, 280, 40 }, "LOAD GAME", false, false },
    { { 500, 385, 280, 40 }, "CONTROLS", false, false },
    { { 500, 435, 280, 40 }, "SETTINGS", false, false },
    { { 500, 485, 280, 40 }, "EXIT", false, false }
};

UIButton pauseButtons[] = {
    { { 500, 210, 280, 40 }, "RESUME GAME", false, false },
    { { 500, 260, 280, 40 }, "SAVE PROGRESS", false, false },
    { { 500, 310, 280, 40 }, "LOAD PREVIOUS", false, false },
    { { 500, 360, 280, 40 }, "SETTINGS", false, false },
    { { 500, 410, 280, 40 }, "MAIN MENU", false, false }
};

UIButton controlsButton = { { 490, 580, 300, 50 }, "BACK", false, false };

UIButton gameOverButtons[] = {
    { { 490, 390, 300, 50 }, "TRY AGAIN", false, false },
    { { 490, 460, 300, 50 }, "MAIN MENU", false, false }
};

UIButton victoryButtons[] = {
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
            corFundo = Fade(THEME_COLOR_BORDER, 0.85f);
            corBorda = THEME_COLOR_MAIN; // Cyan brilhante no hover
            corTexto = THEME_COLOR_MAIN;
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


// ============================================================================
// 1. TELA: MENU PRINCIPAL
// ============================================================================
void DrawTelaMenu(GameState *game, Font font, float time)
{
    // Fundo verde-petóleo biológico (organismo vivo)
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
                           THEME_COLOR_BG_DARK, THEME_COLOR_BG_LIGHT);

    // Desenha as partículas de fundo (flutuando de baixo para cima)
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (game->particles[i].active)
        {
            float s = game->particles[i].size;
            DrawRectangleV((Vector2){ game->particles[i].position.x - s, game->particles[i].position.y - s }, (Vector2){ s * 2.0f, s * 2.0f }, Fade(game->particles[i].color, game->particles[i].lifeTime / game->particles[i].maxLifeTime));
        }
    }

    // Painel Central Sci-Fi (Dashboard)
    DrawSciFiBox((Rectangle){ 430, 45, 420, 570 }, THEME_COLOR_MAIN);

    // Título flutuante dinâmico (com cálculo de seno baseado no tempo)
    float titleOffsetY = sinf(time * 2.0f) * 4.0f;
    const char *titulo = "Disease's Doomsday";
    int titleFontSize = 48;
    Vector2 titleSize = MeasureTextEx(font, titulo, (float)titleFontSize, 2.0f);
    
    // Sombra do título
    Vector2 shadowPos = {
        (SCREEN_WIDTH / 2.0f) - (titleSize.x / 2.0f) + 3.0f,
        75.0f + titleOffsetY + 3.0f
    };
    DrawTextEx(font, titulo, shadowPos, (float)titleFontSize, 2.0f, Fade(BLACK, 0.45f));

    // Texto principal (verde lime biológico)
    Vector2 titlePos = {
        (SCREEN_WIDTH / 2.0f) - (titleSize.x / 2.0f),
        75.0f + titleOffsetY
    };
    DrawTextEx(font, titulo, titlePos, (float)titleFontSize, 2.0f, THEME_COLOR_TEXT);

    // Subtítulo
    const char *sub = "Proteja o organismo. Salve o Distrito Federal.";
    int subSizeVal = 16;
    Vector2 subSize = MeasureTextEx(font, sub, (float)subSizeVal, 1.0f);
    DrawTextEx(font, sub, (Vector2){ (SCREEN_WIDTH / 2.0f) - (subSize.x / 2.0f), 135.0f + titleOffsetY }, 
               (float)subSizeVal, 1.0f, Fade((Color){100,200,150,255}, 0.8f));

    // Desenha Campo de Texto para o Nome do Jogador (Y = 200, Altura 40)
    Rectangle nameBounds = { 500, 200, 280, 40 };
    bool nameHover = CheckCollisionPointRec(g_virtualMouse, nameBounds);
    
    Color boxBg = Fade(THEME_COLOR_PANEL, 0.85f);
    Color boxBorder = THEME_COLOR_BORDER;
    
    if (game->nameInputActive)
    {
        boxBorder = THEME_COLOR_MAIN; // Cyan se ativo
    }
    else if (nameHover)
    {
        boxBorder = YELLOW; // Amarelo no hover
    }
    
    DrawRectangleRounded(nameBounds, 0.2f, 6, boxBg);
    DrawRectangleRoundedLines(nameBounds, 0.2f, 6, boxBorder);
    
    // Rótulo acima do campo
    DrawTextEx(font, "Seu Anticorpo:", (Vector2){ nameBounds.x + 5, nameBounds.y - 18 }, 14.0f, 1.0f, GRAY);
    
    // Desenha o texto do nome ou placeholder
    int fontSize = 20;
    if (game->player.name[0] == '\0')
    {
        if (game->nameInputActive)
        {
            const char *cursorStr = ((int)(time * 2.0f) % 2 == 0) ? "|" : "";
            Vector2 textSz = MeasureTextEx(font, cursorStr, (float)fontSize, 1.0f);
            Vector2 textPos = {
                nameBounds.x + (nameBounds.width / 2.0f) - (textSz.x / 2.0f),
                nameBounds.y + (nameBounds.height / 2.0f) - (textSz.y / 2.0f)
            };
            DrawTextEx(font, cursorStr, textPos, (float)fontSize, 1.0f, WHITE);
        }
        else
        {
            Vector2 textSz = MeasureTextEx(font, "NOME DO ANTICORPO...", (float)fontSize, 1.0f);
            Vector2 textPos = {
                nameBounds.x + (nameBounds.width / 2.0f) - (textSz.x / 2.0f),
                nameBounds.y + (nameBounds.height / 2.0f) - (textSz.y / 2.0f)
            };
            DrawTextEx(font, "NOME DO ANTICORPO...", textPos, (float)fontSize, 1.0f, Fade(GRAY, 0.5f));
        }
    }
    else
    {
        char nameWithCursor[32];
        if (game->nameInputActive && ((int)(time * 2.0f) % 2 == 0))
        {
            sprintf(nameWithCursor, "%s|", game->player.name);
        }
        else
        {
            sprintf(nameWithCursor, "%s", game->player.name);
        }
        
        Vector2 textSz = MeasureTextEx(font, nameWithCursor, (float)fontSize, 1.0f);
        Vector2 textPos = {
            nameBounds.x + (nameBounds.width / 2.0f) - (textSz.x / 2.0f),
            nameBounds.y + (nameBounds.height / 2.0f) - (textSz.y / 2.0f)
        };
        DrawTextEx(font, nameWithCursor, textPos, (float)fontSize, 1.0f, WHITE);
    }

    // Se existe arquivo de save em qualquer um dos 3 slots, indica que pode carregar
    bool anySaveExists = AnySaveExistsCached();

    // Desenha Botões do Menu
    for (int i = 0; i < 5; i++)
    {
        DrawButton(menuButtons[i], font, (i == 1) ? anySaveExists : true);
    }

    // Rodapé decorativo no painel
    DrawLineEx((Vector2){ 460, 575 }, (Vector2){ 820, 575 }, 1.0f, Fade(THEME_COLOR_TEXT, 0.25f));
    const char *verText = "SISTEMA: ATIVO v1.0 | Fase 1: A Seringa";
    Vector2 verSize = MeasureTextEx(font, verText, 12.0f, 1.0f);
    DrawTextEx(font, verText, (Vector2){ 640.0f - verSize.x / 2.0f, 585.0f }, 12.0f, 1.0f, Fade(THEME_COLOR_TEXT, 0.5f));

    // Rodapé
    DrawTextEx(font, "ESC para pausar | Powered by Raylib", (Vector2){ 20, SCREEN_HEIGHT - 35 }, 16.0f, 1.0f, DARKGRAY);
}



// ============================================================================
// 2. TELA: CONTROLES / CRÉDITOS
// ============================================================================
void DrawTelaControles(GameState *game, Font font)
{
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
                           (Color){ 6, 14, 10, 255 }, (Color){ 10, 22, 16, 255 });

    DrawTextEx(font, "CONTROLES & REGRAS", (Vector2){ 430, 60 }, 42.0f, 1.5f, THEME_COLOR_TEXT);

    // Painel de Instruções centralizado
    DrawRectangleRounded((Rectangle){ 240, 140, 800, 400 }, 0.05f, 6, Fade(BLACK, 0.45f));
    DrawRectangleRoundedLines((Rectangle){ 240, 140, 800, 400 }, 0.05f, 6, THEME_COLOR_BORDER);

    int startY = 170;
    int spacing = 38;
    
    // Atalhos
    DrawTextEx(font, "W, A, S, D ou SETAS", (Vector2){ 280, (float)startY }, 22.0f, 1.0f, YELLOW);
    DrawTextEx(font, "- Mova o Anticorpo pelo campo de batalha", (Vector2){ 480, (float)startY }, 22.0f, 1.0f, WHITE);
    
    DrawTextEx(font, "ESPAÇO ou CLIQUE ESQUERDO", (Vector2){ 280, (float)(startY + spacing) }, 22.0f, 1.0f, YELLOW);
    DrawTextEx(font, "- Ataque (acerta ao redor com a Lâmina)", (Vector2){ 580, (float)(startY + spacing) }, 22.0f, 1.0f, WHITE);

    DrawTextEx(font, "TECLAS 1, 2, 3 e 4", (Vector2){ 280, (float)(startY + spacing * 2) }, 22.0f, 1.0f, YELLOW);
    DrawTextEx(font, "- Troca de arma (Lâmina, Fuzil, Granada, BFG)", (Vector2){ 580, (float)(startY + spacing * 2) }, 22.0f, 1.0f, WHITE);

    DrawTextEx(font, "E", (Vector2){ 280, (float)(startY + spacing * 3) }, 22.0f, 1.0f, YELLOW);
    DrawTextEx(font, "- Usa uma Poção de Vida (cura 50%)", (Vector2){ 580, (float)(startY + spacing * 3) }, 22.0f, 1.0f, WHITE);

    DrawTextEx(font, "ESC", (Vector2){ 280, (float)(startY + spacing * 4) }, 22.0f, 1.0f, YELLOW);
    DrawTextEx(font, "- Pausar / Voltar ao Menu", (Vector2){ 580, (float)(startY + spacing * 4) }, 22.0f, 1.0f, WHITE);

    DrawTextEx(font, "F5 / F9", (Vector2){ 280, (float)(startY + spacing * 5) }, 22.0f, 1.0f, YELLOW);
    DrawTextEx(font, "- Quicksave / Quickload (Slot 1)", (Vector2){ 580, (float)(startY + spacing * 5) }, 22.0f, 1.0f, WHITE);

    // Legenda de Itens
    DrawTextEx(font, "POWER-UPS (BRILHANTES NO MAPA):", (Vector2){ 280, (float)(startY + spacing * 6.3f) }, 20.0f, 1.0f, GOLD);
    DrawTextEx(font, "Cura (+35 HP) | Velocidade (+60%) | Escudo (invencível) | Dano Duplo (x2)", (Vector2){ 280, (float)(startY + spacing * 7.2f) }, 18.0f, 1.0f, LIGHTGRAY);

    // Botão voltar
    DrawButton(controlsButton, font, true);
}




// ============================================================================
// 3. TELA: PAUSA (OVERLAY)
// ============================================================================
void DrawTelaPausa(GameState *game, Font font)
{
    // Escurece a tela de fundo da gameplay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.65f));

    float animDuration = 0.5f;
    float t = game->uiAnimTimer / animDuration;
    if (t > 1.0f) t = 1.0f;
    float ease = EaseOutCubic(t);
    float offsetY = (1.0f - ease) * -800.0f; // Começa de cima

    rlPushMatrix();
    rlTranslatef(0.0f, offsetY, 0.0f);

    // Caixa de diálogo central (Glassmorphism místico do void)
    DrawRectangleRounded((Rectangle){ 420, 130, 440, 350 }, 0.05f, 6, Fade((Color){ 12, 8, 22, 255 }, 0.92f));
    DrawRectangleRoundedLines((Rectangle){ 420, 130, 440, 350 }, 0.05f, 6, THEME_COLOR_BORDER);

    const char *pauseTxt = "GAME PAUSED";
    Vector2 pauseSize = MeasureTextEx(font, pauseTxt, 28.0f, 1.0f);
    DrawTextEx(font, pauseTxt, (Vector2){ (SCREEN_WIDTH / 2.0f) - (pauseSize.x / 2.0f), 155.0f }, 28.0f, 1.0f, THEME_COLOR_MAIN);

    for (int i = 0; i < 5; i++)
    {
        DrawButton(pauseButtons[i], font, true);
    }
    
    rlPopMatrix();
}



// ============================================================================
// 4. TELA: GAME OVER
// ============================================================================
void DrawTelaGameOver(GameState *game, Font font)
{
    // Fundo vermelho escuro/preto degradê do void
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
                           (Color){ 20, 6, 12, 255 }, (Color){ 10, 4, 8, 255 });

    // Título pulsante
    float pulse = 1.0f + sinf((float)GetTime() * 4.0f) * 0.05f;
    const char *txt = "GAME OVER";
    int fSize = 65;
    Vector2 txtSize = MeasureTextEx(font, txt, (float)fSize, 2.0f);
    Vector2 txtPos = {
        (SCREEN_WIDTH / 2.0f) - (txtSize.x * pulse / 2.0f),
        120.0f - (txtSize.y * pulse / 2.0f)
    };
    DrawTextEx(font, txt, txtPos, (float)fSize * pulse, 2.0f, RED);

    // Painel de estatísticas (Estilo SciFi)
    DrawSciFiBox((Rectangle){ 400, 200, 480, 160 }, MAROON);

    DrawTextEx(font, TextFormat("Pontuação Final: %d", game->player.score), (Vector2){ 440, 225 }, 22.0f, 1.0f, WHITE);
    DrawTextEx(font, TextFormat("Nível Final: Lvl %d", game->player.level), (Vector2){ 440, 260 }, 22.0f, 1.0f, WHITE);
    DrawTextEx(font, TextFormat("Patógenos Eliminados: %d", game->totalEnemiesKilled), (Vector2){ 440, 295 }, 22.0f, 1.0f, WHITE);

    // Botões
    for (int i = 0; i < 2; i++)
    {
        DrawButton(gameOverButtons[i], font, true);
    }
}



// ============================================================================
// 5. TELA: VITÓRIA
// ============================================================================
void DrawTelaVitoria(GameState *game, Font font)
{
    // Fundo azul escuro do espaço sideral degradê
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
                           (Color){ 10, 16, 36, 255 }, (Color){ 6, 8, 20, 255 });

    // Efeito de estrelas/partículas subindo
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (game->particles[i].active)
        {
            float s = game->particles[i].size;
            DrawRectangleV((Vector2){ game->particles[i].position.x - s, game->particles[i].position.y - s }, (Vector2){ s * 2.0f, s * 2.0f }, Fade(game->particles[i].color, game->particles[i].lifeTime / game->particles[i].maxLifeTime));
        }
    }

    // Título
    float pulse = 1.0f + sinf((float)GetTime() * 3.5f) * 0.04f;
    const char *txt = "ORGANISMO CURADO!";
    int fSize = 65;
    Vector2 txtSize = MeasureTextEx(font, txt, (float)fSize, 2.0f);
    Vector2 txtPos = {
        (SCREEN_WIDTH / 2.0f) - (txtSize.x * pulse / 2.0f),
        120.0f - (txtSize.y * pulse / 2.0f)
    };
    DrawTextEx(font, txt, txtPos, (float)fSize * pulse, 2.0f, THEME_COLOR_TEXT);

    // Parabéns
    const char *congrats = "Você erradicou todas as infecções do Distrito Federal!";
    Vector2 congratsSize = MeasureTextEx(font, congrats, 18.0f, 1.0f);
    DrawTextEx(font, congrats, (Vector2){ (SCREEN_WIDTH / 2.0f) - (congratsSize.x / 2.0f), 165.0f }, 18.0f, 1.0f, (Color){ 100, 220, 160, 255 });

    // Painel de estatísticas (Estilo SciFi)
    DrawSciFiBox((Rectangle){ 400, 200, 480, 160 }, GOLD);

    DrawTextEx(font, TextFormat("Pontuação Final: %d", game->player.score), (Vector2){ 440, 225 }, 22.0f, 1.0f, GOLD);
    DrawTextEx(font, TextFormat("Nível Final: Lvl %d", game->player.level), (Vector2){ 440, 260 }, 22.0f, 1.0f, WHITE);
    DrawTextEx(font, TextFormat("Total de Patógenos: %d", game->totalEnemiesKilled), (Vector2){ 440, 295 }, 22.0f, 1.0f, WHITE);

    // Botões
    for (int i = 0; i < 2; i++)
    {
        DrawButton(victoryButtons[i], font, true);
    }
}



// ============================================================================
// 6. MINI-MAPA & HUD DA GAMEPLAY
// ============================================================================
// Auxiliary Sci-fi corner brackets container helper
void DrawSciFiBox(Rectangle r, Color col)
{
    // Deep dark translucent panel background (cockpit glass style)
    DrawRectangleRec(r, Fade((Color){ 8, 6, 16, 255 }, 0.65f));
    
    // Sleek thin border
    DrawRectangleLinesEx(r, 1.0f, Fade(col, 0.25f));
    
    // High-tech corner bracket markings
    float len = 10.0f;
    float thickness = 2.0f;
    // Top-left corner
    DrawLineEx((Vector2){ r.x, r.y }, (Vector2){ r.x + len, r.y }, thickness, col);
    DrawLineEx((Vector2){ r.x, r.y }, (Vector2){ r.x, r.y + len }, thickness, col);
    // Top-right corner
    DrawLineEx((Vector2){ r.x + r.width, r.y }, (Vector2){ r.x + r.width - len, r.y }, thickness, col);
    DrawLineEx((Vector2){ r.x + r.width, r.y }, (Vector2){ r.x + r.width, r.y + len }, thickness, col);
    // Bottom-left corner
    DrawLineEx((Vector2){ r.x, r.y + r.height }, (Vector2){ r.x + len, r.y + r.height }, thickness, col);
    DrawLineEx((Vector2){ r.x, r.y + r.height }, (Vector2){ r.x, r.y + r.height - len }, thickness, col);
    // Bottom-right corner
    DrawLineEx((Vector2){ r.x + r.width, r.y + r.height }, (Vector2){ r.x + r.width - len, r.y + r.height }, thickness, col);
    DrawLineEx((Vector2){ r.x + r.width, r.y + r.height }, (Vector2){ r.x + r.width, r.y + r.height - len }, thickness, col);
}

// ============================================================================
// 8. TELAS DE SELEÇÃO DE SAVE E LOAD
// ============================================================================

void DrawTelaSaveSelect(GameState *game, Font font, Vector2 mouse, Texture2D slotTextures[3], bool slotTexturesLoaded[3])
{
    // Fundo escuro translúcido para revelar a run desfocada/limpa no fundo
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade((Color){ 10, 8, 22, 255 }, 0.75f));

    // Título
    const char *titulo = "SAVE PROGRESS";
    Vector2 titleSize = MeasureTextEx(font, titulo, 42.0f, 1.5f);
    DrawTextEx(font, titulo, (Vector2){ (SCREEN_WIDTH / 2.0f) - (titleSize.x / 2.0f), 40.0f }, 42.0f, 1.5f, SKYBLUE);
    
    const char *sub = "Choose a slot to save the current game";
    Vector2 subSize = MeasureTextEx(font, sub, 18.0f, 1.0f);
    DrawTextEx(font, sub, (Vector2){ (SCREEN_WIDTH / 2.0f) - (subSize.x / 2.0f), 90.0f }, 18.0f, 1.0f, GRAY);

    // 3 Slots Cards
    for (int i = 0; i < 3; i++)
    {
        float cardX = 80.0f + (float)i * 400.0f;
        Rectangle cardBounds = { cardX, 140, 320, 430 };

        bool hover = CheckCollisionPointRec(mouse, cardBounds);

        Color cardBg = Fade((Color){ 20, 18, 32, 255 }, 0.85f);
        Color borderCol = hover ? THEME_COLOR_MAIN : THEME_COLOR_BORDER;

        DrawRectangleRounded(cardBounds, 0.05f, 6, cardBg);
        DrawRectangleRoundedLines(cardBounds, 0.05f, 6, borderCol);

        // Screenshot Box (16:9 ratio, e.g. 280 x 158)
        Rectangle imgBounds = { cardX + 20, 160, 280, 158 };
        DrawRectangleRec(imgBounds, BLACK);

        if (slotTexturesLoaded[i])
        {
            DrawTexturePro(slotTextures[i],
                           (Rectangle){ 0, 0, (float)slotTextures[i].width, (float)slotTextures[i].height },
                           imgBounds, (Vector2){ 0, 0 }, 0.0f, WHITE);
        }
        else
        {
            DrawRectangleLinesEx(imgBounds, 1.0f, GRAY);
            Vector2 textSz = MeasureTextEx(font, "NO PREVIEW", 18.0f, 1.0f);
            DrawTextEx(font, "NO PREVIEW",
                       (Vector2){ imgBounds.x + imgBounds.width / 2.0f - textSz.x / 2.0f, imgBounds.y + imgBounds.height / 2.0f - textSz.y / 2.0f },
                       18.0f, 1.0f, DARKGRAY);
        }

        // Metadados do Slot
        SaveSlotMeta meta = game->slotsMeta[i];

        // Rótulo do Slot
        if (i == 0)
        {
            DrawTextEx(font, "SLOT 1 (DEFAULT)", (Vector2){ cardX + 20, 335 }, 22.0f, 1.0f, GOLD);
        }
        else
        {
            DrawTextEx(font, TextFormat("SLOT %d", i + 1), (Vector2){ cardX + 20, 335 }, 22.0f, 1.0f, GOLD);
        }

        if (meta.exists)
        {
            DrawTextEx(font, TextFormat("Hero: %s", meta.name), (Vector2){ cardX + 20, 365 }, 18.0f, 1.0f, WHITE);
            DrawTextEx(font, TextFormat("Level: Lvl %d", meta.level), (Vector2){ cardX + 20, 390 }, 18.0f, 1.0f, WHITE);
            DrawTextEx(font, TextFormat("Wave: %d / 5", meta.wave), (Vector2){ cardX + 20, 415 }, 18.0f, 1.0f, WHITE);
            DrawTextEx(font, TextFormat("Score: %d", meta.score), (Vector2){ cardX + 20, 440 }, 18.0f, 1.0f, WHITE);
            DrawTextEx(font, TextFormat("Date: %s", meta.date), (Vector2){ cardX + 20, 470 }, 14.0f, 1.0f, GRAY);

            // Botão Apagar Save
            Rectangle deleteBounds = { cardX + 20, 495, 280, 35 };
            bool deleteHover = CheckCollisionPointRec(mouse, deleteBounds);
            Color delBg = deleteHover ? RED : Fade(RED, 0.3f);
            DrawRectangleRounded(deleteBounds, 0.25f, 6, delBg);
            DrawRectangleRoundedLines(deleteBounds, 0.25f, 6, RED);
            
            int delFontSize = 16;
            Vector2 delTextSz = MeasureTextEx(font, "DELETE SAVE", (float)delFontSize, 1.0f);
            DrawTextEx(font, "DELETE SAVE", (Vector2){ deleteBounds.x + deleteBounds.width/2.0f - delTextSz.x/2.0f, deleteBounds.y + deleteBounds.height/2.0f - delTextSz.y/2.0f }, (float)delFontSize, 1.0f, WHITE);

            // Hover do card como um todo (evitando desenhar se mouse está sobre o botão apagar)
            if (hover && !deleteHover)
            {
                DrawTextEx(font, "CLICK TO OVERWRITE", (Vector2){ cardX + 20, 545 }, 14.0f, 1.0f, RED);
            }
        }
        else
        {
            DrawTextEx(font, "EMPTY SLOT", (Vector2){ cardX + 20, 365 }, 20.0f, 1.0f, DARKGRAY);

            if (hover)
            {
                DrawTextEx(font, "CLICK TO SAVE GAME", (Vector2){ cardX + 20, 545 }, 14.0f, 1.0f, GREEN);
            }
        }
    }

    // Botão Voltar
    UIButton btnVoltar = { { 490, 600, 300, 50 }, "BACK", false, false };
    btnVoltar.hover = CheckCollisionPointRec(mouse, btnVoltar.bounds);
    DrawButton(btnVoltar, font, true);
}

void DrawTelaLoadSelect(GameState *game, Font font, Vector2 mouse, Texture2D slotTextures[3], bool slotTexturesLoaded[3])
{
    // Fundo escuro translúcido para revelar a run desfocada/limpa no fundo
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade((Color){ 10, 8, 22, 255 }, 0.75f));

    // Título
    const char *titulo = "LOAD GAME";
    Vector2 titleSize = MeasureTextEx(font, titulo, 42.0f, 1.5f);
    DrawTextEx(font, titulo, (Vector2){ (SCREEN_WIDTH / 2.0f) - (titleSize.x / 2.0f), 40.0f }, 42.0f, 1.5f, SKYBLUE);
    
    const char *sub = "Choose a save to continue your journey";
    Vector2 subSize = MeasureTextEx(font, sub, 18.0f, 1.0f);
    DrawTextEx(font, sub, (Vector2){ (SCREEN_WIDTH / 2.0f) - (subSize.x / 2.0f), 90.0f }, 18.0f, 1.0f, GRAY);

    // 3 Slots Cards
    for (int i = 0; i < 3; i++)
    {
        float cardX = 80.0f + (float)i * 400.0f;
        Rectangle cardBounds = { cardX, 140, 320, 430 };

        bool hover = CheckCollisionPointRec(mouse, cardBounds);
        SaveSlotMeta meta = game->slotsMeta[i];

        Color cardBg = Fade((Color){ 20, 18, 32, 255 }, 0.85f);
        Color borderCol = (hover && meta.exists) ? THEME_COLOR_MAIN : THEME_COLOR_BORDER;

        if (!meta.exists)
        {
            borderCol = Fade(borderCol, 0.4f);
            cardBg = Fade(cardBg, 0.5f);
        }

        DrawRectangleRounded(cardBounds, 0.05f, 6, cardBg);
        DrawRectangleRoundedLines(cardBounds, 0.05f, 6, borderCol);

        // Screenshot Box (16:9 ratio, e.g. 280 x 158)
        Rectangle imgBounds = { cardX + 20, 160, 280, 158 };
        DrawRectangleRec(imgBounds, BLACK);

        if (slotTexturesLoaded[i])
        {
            DrawTexturePro(slotTextures[i],
                           (Rectangle){ 0, 0, (float)slotTextures[i].width, (float)slotTextures[i].height },
                           imgBounds, (Vector2){ 0, 0 }, 0.0f, WHITE);
        }
        else
        {
            DrawRectangleLinesEx(imgBounds, 1.0f, DARKGRAY);
            Vector2 textSz = MeasureTextEx(font, "NO PREVIEW", 18.0f, 1.0f);
            DrawTextEx(font, "NO PREVIEW",
                       (Vector2){ imgBounds.x + imgBounds.width / 2.0f - textSz.x / 2.0f, imgBounds.y + imgBounds.height / 2.0f - textSz.y / 2.0f },
                       18.0f, 1.0f, DARKGRAY);
        }

        // Rótulo do Slot
        if (i == 0)
        {
            DrawTextEx(font, "SLOT 1 (DEFAULT)", (Vector2){ cardX + 20, 335 }, 22.0f, 1.0f, GOLD);
        }
        else
        {
            DrawTextEx(font, TextFormat("SLOT %d", i + 1), (Vector2){ cardX + 20, 335 }, 22.0f, 1.0f, GOLD);
        }

        if (meta.exists)
        {
            DrawTextEx(font, TextFormat("Hero: %s", meta.name), (Vector2){ cardX + 20, 365 }, 18.0f, 1.0f, WHITE);
            DrawTextEx(font, TextFormat("Level: Lvl %d", meta.level), (Vector2){ cardX + 20, 390 }, 18.0f, 1.0f, WHITE);
            DrawTextEx(font, TextFormat("Wave: %d / 5", meta.wave), (Vector2){ cardX + 20, 415 }, 18.0f, 1.0f, WHITE);
            DrawTextEx(font, TextFormat("Score: %d", meta.score), (Vector2){ cardX + 20, 440 }, 18.0f, 1.0f, WHITE);
            DrawTextEx(font, TextFormat("Date: %s", meta.date), (Vector2){ cardX + 20, 470 }, 14.0f, 1.0f, GRAY);

            // Botão Apagar Save
            Rectangle deleteBounds = { cardX + 20, 495, 280, 35 };
            bool deleteHover = CheckCollisionPointRec(mouse, deleteBounds);
            Color delBg = deleteHover ? RED : Fade(RED, 0.3f);
            DrawRectangleRounded(deleteBounds, 0.25f, 6, delBg);
            DrawRectangleRoundedLines(deleteBounds, 0.25f, 6, RED);
            
            int delFontSize = 16;
            Vector2 delTextSz = MeasureTextEx(font, "DELETE SAVE", (float)delFontSize, 1.0f);
            DrawTextEx(font, "DELETE SAVE", (Vector2){ deleteBounds.x + deleteBounds.width/2.0f - delTextSz.x/2.0f, deleteBounds.y + deleteBounds.height/2.0f - delTextSz.y/2.0f }, (float)delFontSize, 1.0f, WHITE);

            // Hover do card como um todo
            if (hover && !deleteHover)
            {
                DrawTextEx(font, "CLICK TO LOAD", (Vector2){ cardX + 20, 545 }, 14.0f, 1.0f, SKYBLUE);
            }
        }
        else
        {
            DrawTextEx(font, "EMPTY SLOT", (Vector2){ cardX + 20, 365 }, 20.0f, 1.0f, DARKGRAY);
        }
    }

    // Botão Voltar
    UIButton btnVoltar = { { 490, 600, 300, 50 }, "BACK", false, false };
    btnVoltar.hover = CheckCollisionPointRec(mouse, btnVoltar.bounds);
    DrawButton(btnVoltar, font, true);
}






// 10. TELA: SETTINGS
// ============================================================================
UIButton settingsBtnVoltar = { { 490, 600, 300, 50 }, "BACK", false, false };

void DrawTelaSettings(GameState *game, Font font)
{
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
                           (Color){ 10, 8, 20, 255 }, (Color){ 20, 12, 36, 255 });

    DrawTextEx(font, "SETTINGS", (Vector2){ 540, 60 }, 42.0f, 1.5f, SKYBLUE);

    DrawSciFiBox((Rectangle){ 340, 150, 600, 400 }, THEME_COLOR_MAIN);

    DrawTextEx(font, "AUDIO", (Vector2){ 380, 170 }, 28.0f, 1.0f, YELLOW);
    DrawLine(380, 205, 900, 205, Fade(YELLOW, 0.5f));

    DrawTextEx(font, "MASTER VOLUME", (Vector2){ 380, 230 }, 24.0f, 1.0f, WHITE);

    Rectangle sliderBg = { 600, 230, 300, 20 };
    Rectangle sliderFill = { 600, 230, 300 * game->masterVolume, 20 };

    DrawRectangleRec(sliderBg, Fade(BLACK, 0.6f));
    DrawRectangleRec(sliderFill, THEME_COLOR_MAIN);
    DrawRectangleLinesEx(sliderBg, 2.0f, THEME_COLOR_BORDER);

    char volText[16];
    sprintf(volText, "%d%%", (int)(game->masterVolume * 100));
    DrawTextEx(font, volText, (Vector2){ 920, 228 }, 24.0f, 1.0f, WHITE);

    // ------------------------------------------------------------------------
    // SELETORES DE SKIN (personagem e arma)
    // ------------------------------------------------------------------------
    DrawTextEx(font, "APARENCIA", (Vector2){ 380, 290 }, 28.0f, 1.0f, YELLOW);
    DrawLine(380, 325, 900, 325, Fade(YELLOW, 0.5f));

    const char *rowLabels[2] = { "SKIN DO ANTICORPO", "SKIN DA ARMA" };
    for (int row = 0; row < 2; row++)
    {
        float y = 355.0f + row * 70.0f;
        DrawTextEx(font, rowLabels[row], (Vector2){ 380, y + 8 }, 20.0f, 1.0f, WHITE);

        // Setas < >
        Rectangle btnPrev = { 600, y, 40, 40 };
        Rectangle btnNext = { 860, y, 40, 40 };
        bool hovPrev = CheckCollisionPointRec(g_virtualMouse, btnPrev);
        bool hovNext = CheckCollisionPointRec(g_virtualMouse, btnNext);
        DrawRectangleRounded(btnPrev, 0.3f, 6, Fade(THEME_COLOR_BORDER, hovPrev ? 0.9f : 0.5f));
        DrawRectangleRounded(btnNext, 0.3f, 6, Fade(THEME_COLOR_BORDER, hovNext ? 0.9f : 0.5f));
        DrawTextEx(font, "<", (Vector2){ btnPrev.x + 14, btnPrev.y + 8 }, 24.0f, 1.0f, hovPrev ? THEME_COLOR_MAIN : WHITE);
        DrawTextEx(font, ">", (Vector2){ btnNext.x + 14, btnNext.y + 8 }, 24.0f, 1.0f, hovNext ? THEME_COLOR_MAIN : WHITE);

        // Nome da skin atual + amostra de cor
        const char *skinName;
        Color swatch;
        if (row == 0)
        {
            skinName = PlayerSkinName(game->player.skinId);
            swatch = (game->player.skinId == 1) ? (Color){ 225, 55, 60, 255 }
                   : (game->player.skinId == 2) ? (Color){ 130, 220, 40, 255 }
                                                : (Color){ 235, 240, 250, 255 };
        }
        else
        {
            skinName = WeaponSkinName(game->player.weaponSkinId);
            swatch = WeaponSkinPrimary(game->player.weaponSkinId);
        }

        Rectangle nameBox = { 650, y, 200, 40 };
        DrawRectangleRounded(nameBox, 0.2f, 6, Fade(BLACK, 0.55f));
        DrawRectangleRoundedLines(nameBox, 0.2f, 6, THEME_COLOR_BORDER);
        DrawCircle((int)(nameBox.x + 22), (int)(y + 20), 10.0f, swatch);
        Vector2 nameSz = MeasureTextEx(font, skinName, 18.0f, 1.0f);
        DrawTextEx(font, skinName, (Vector2){ nameBox.x + 110.0f - nameSz.x / 2.0f + 10.0f, y + 11 }, 18.0f, 1.0f, WHITE);
    }

    DrawTextEx(font, "As skins sao salvas automaticamente.", (Vector2){ 380, 505 }, 14.0f, 1.0f, GRAY);

    DrawButton(settingsBtnVoltar, font, true);
}



// ============================================================================
// 11. TELA: LOADING (CARREGAMENTO)
// ============================================================================
static const char *loadingTips[] = {
    "DICA DE SAUDE: A vacina contra a gripe (Influenza) ajuda a prevenir complicacoes graves e deve ser tomada anualmente na UBS.",
    "DICA DE SAUDE: Para combater a Dengue, permita a entrada dos agentes da Vigilancia Ambiental em sua residencia para inspecao.",
    "DICA DE SAUDE: O Distrito Federal possui diversas Unidades Basicas de Saude (UBS) que oferecem atendimento e vacinacao gratuitos.",
    "DICA DE SAUDE: Lavar as maos frequentemente com agua e sabao e uma das maneiras mais eficazes de prevenir a transmissao de patogenos.",
    "DICA DE SAUDE: Informe a Vigilancia Ambiental sobre lotes vagos com acumulo de lixo ou agua parada na sua regiao administrativa."
};

void DrawTelaLoading(GameState *game, Font font)
{
    // ------------------------------------------------------------------------
    // EFEITO "INJEÇÃO": na transição tutorial -> gameplay o personagem é
    // comprimido pela ampola e a tela treme progressivamente (smoothstep).
    // ------------------------------------------------------------------------
    float fxProgress = 0.0f;
    bool fxActive = game->syringeTransitionFX;
    if (fxActive && game->loadingDuration > 0.0f)
    {
        fxProgress = game->loadingTimer / game->loadingDuration;
        if (fxProgress > 1.0f) fxProgress = 1.0f;
    }

    // shakeIntensity = smoothstep(0,1,progress) * maxShake
    float maxShake = 14.0f;
    float shakeIntensity = fxActive ? SmoothStep01(fxProgress) * maxShake : 0.0f;
    Vector2 shakeOff = { 0.0f, 0.0f };
    if (shakeIntensity > 0.1f)
    {
        shakeOff.x = (float)GetRandomValue(-100, 100) / 100.0f * shakeIntensity;
        shakeOff.y = (float)GetRandomValue(-100, 100) / 100.0f * shakeIntensity;
    }

    rlPushMatrix();
    rlTranslatef(shakeOff.x, shakeOff.y, 0.0f);

    // Fundo verde-petróleo biológico escuro degradê
    DrawRectangleGradientV(-20, -20, SCREEN_WIDTH + 40, SCREEN_HEIGHT + 40,
                           (Color){ 8, 20, 12, 255 }, (Color){ 10, 28, 20, 255 });

    // Células/partículas decorativas de fundo que simulam o sangue/tecido
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (game->particles[i].active)
        {
            float s = game->particles[i].size;
            DrawRectangleV((Vector2){ game->particles[i].position.x - s, game->particles[i].position.y - s }, (Vector2){ s * 2.0f, s * 2.0f }, Fade(game->particles[i].color, game->particles[i].lifeTime / game->particles[i].maxLifeTime * 0.15f));
        }
    }

    // Título (muda durante a injeção da seringa)
    const char *titulo = fxActive ? "INJETANDO NA CORRENTE SANGUINEA..."
                                  : "CARREGANDO SISTEMA IMUNOLOGICO...";
    Vector2 titleSz = MeasureTextEx(font, titulo, 26.0f, 1.2f);
    DrawTextEx(font, titulo, (Vector2){ (SCREEN_WIDTH / 2.0f) - (titleSz.x / 2.0f), 240.0f }, 26.0f, 1.2f, THEME_COLOR_MAIN);

    // ------------------------------------------------------------------------
    // CENA DA COMPRESSÃO: ampola apertando o herói (apenas no efeito de injeção)
    // ------------------------------------------------------------------------
    if (fxActive)
    {
        float cx = SCREEN_WIDTH / 2.0f;
        float cy = 140.0f;
        float ease = SmoothStep01(fxProgress);

        // Paredes da ampola convergindo (de 180px de abertura para 36px)
        float gap = 90.0f - ease * 72.0f;
        Color wallCol = (Color){ 155, 175, 192, 255 };
        DrawRectangleRounded((Rectangle){ cx - 160.0f, cy - gap - 26.0f, 320.0f, 22.0f }, 0.5f, 6, wallCol);
        DrawRectangleRounded((Rectangle){ cx - 160.0f, cy + gap + 4.0f, 320.0f, 22.0f }, 0.5f, 6, wallCol);
        DrawRectangleLinesEx((Rectangle){ cx - 160.0f, cy - gap - 26.0f, 320.0f, gap * 2.0f + 52.0f }, 2.0f, Fade(THEME_COLOR_MAIN, 0.35f));

        // Herói sendo comprimido (squash crescente)
        float squashY = 1.0f - ease * 0.62f;
        float squashX = 1.0f + ease * 0.85f;
        float heroR = 26.0f;
        Color heroCol = (Color){ 235, 240, 250, 255 };
        DrawEllipse((int)cx, (int)cy, heroR * squashX, heroR * squashY, heroCol);
        DrawEllipseLines((int)cx, (int)cy, heroR * squashX, heroR * squashY, THEME_COLOR_MAIN);
        // Visor do elmo
        DrawRectangle((int)(cx - 6.0f * squashX), (int)(cy - 3.0f * squashY), (int)(12.0f * squashX), (int)(5.0f * squashY), BLACK);

        // Linhas de pressão / gotículas escapando conforme aperta
        if (ease > 0.25f)
        {
            float t = (float)GetTime();
            for (int i = 0; i < 6; i++)
            {
                float px = cx + sinf(t * 6.0f + i * 1.7f) * (40.0f + i * 18.0f);
                float py = cy + ((i % 2 == 0) ? -(gap + 34.0f) : (gap + 30.0f));
                DrawCircleV((Vector2){ px, py }, 2.0f + (i % 3), Fade((Color){ 0, 200, 255, 255 }, 0.5f * ease));
            }
        }
    }

    // Barra de progresso (Fundo)
    Rectangle progressBg = { 340, 310, 600, 30 };
    DrawRectangleRounded(progressBg, 0.4f, 6, Fade(BLACK, 0.6f));
    DrawRectangleRoundedLines(progressBg, 0.4f, 6, THEME_COLOR_BORDER);

    // Barra de progresso (Preenchimento)
    float pct = game->loadingDuration > 0.0f ? (game->loadingTimer / game->loadingDuration) : 0.0f;
    if (pct > 1.0f) pct = 1.0f;
    Rectangle progressFill = { 340, 310, 600 * pct, 30 };
    if (pct > 0.02f)
    {
        DrawRectangleRounded(progressFill, 0.4f, 6, THEME_COLOR_MAIN);
    }

    // Porcentagem
    char pctText[16];
    sprintf(pctText, "%d%%", (int)(pct * 100));
    Vector2 pctSz = MeasureTextEx(font, pctText, 18.0f, 1.0f);
    DrawTextEx(font, pctText, (Vector2){ (SCREEN_WIDTH / 2.0f) - (pctSz.x / 2.0f), 316.0f }, 18.0f, 1.0f, BLACK);

    // Painel da dica educativa (Sci-fi Box)
    Rectangle tipBox = { 140, 480, 1000, 140 };
    DrawSciFiBox(tipBox, (Color){ 0, 200, 100, 255 });

    // Título da dica
    const char *tipTitle = "CONSELHO DE SAUDE PUBLICA DO DF";
    Vector2 tipTitleSz = MeasureTextEx(font, tipTitle, 16.0f, 1.0f);
    DrawTextEx(font, tipTitle, (Vector2){ (SCREEN_WIDTH / 2.0f) - (tipTitleSz.x / 2.0f), 500.0f }, 16.0f, 1.0f, (Color){ 0, 220, 120, 255 });

    // Texto da dica (com wrapping em 2 linhas)
    int tipIndex = game->loadingTip;
    if (tipIndex < 0 || tipIndex >= 5) tipIndex = 0;
    const char *tipText = loadingTips[tipIndex];
    
    char line1[256] = {0};
    char line2[256] = {0};
    int len = strlen(tipText);
    int splitIdx = len / 2;
    // Encontra o espaço mais próximo do meio
    while (splitIdx > 0 && tipText[splitIdx] != ' ') {
        splitIdx--;
    }
    if (splitIdx > 0) {
        strncpy(line1, tipText, splitIdx);
        strcpy(line2, tipText + splitIdx + 1);
    } else {
        strcpy(line1, tipText);
    }

    Vector2 line1Sz = MeasureTextEx(font, line1, 15.0f, 1.0f);
    DrawTextEx(font, line1, (Vector2){ (SCREEN_WIDTH / 2.0f) - (line1Sz.x / 2.0f), 535.0f }, 15.0f, 1.0f, WHITE);

    if (line2[0] != '\0') {
        Vector2 line2Sz = MeasureTextEx(font, line2, 15.0f, 1.0f);
        DrawTextEx(font, line2, (Vector2){ (SCREEN_WIDTH / 2.0f) - (line2Sz.x / 2.0f), 565.0f }, 15.0f, 1.0f, WHITE);
    }

    rlPopMatrix();

    // Flash branco final da injeção (últimos 12% do loading), sem tremor
    if (fxActive && fxProgress > 0.88f)
    {
        float flash = (fxProgress - 0.88f) / 0.12f;
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(WHITE, flash * 0.85f));
    }
}
