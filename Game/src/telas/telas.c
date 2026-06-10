// telas.c
// Implementação do HUD, minimapa, efeitos visuais e menus interativos.
#include "../../include/telas.h"
#include "../../include/gameplay.h"
#include "../../include/asset_manager.h"
#include "raymath.h"
#include "../../Assets/Maps/map_seringa.h"
#include "../../Assets/Maps/map_organismo.h"
#include "../../Assets/@models/player_model.h"
#include "../../Assets/@models/enemy_model.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

extern Vector2 g_virtualMouse;

static void DrawSciFiBox(Rectangle r, Color col);

// ============================================================================
// DEFINIÇÃO DOS BOTÕES DAS TELAS (GLOBAIS DA UI)
// ============================================================================
UIButton menuButtons[] = {
    { { 500, 260, 280, 40 }, "NEW GAME", false, false },
    { { 500, 310, 280, 40 }, "LOAD GAME", false, false },
    { { 500, 360, 280, 40 }, "SKINS", false, false },
    { { 500, 410, 280, 40 }, "CONTROLS", false, false },
    { { 500, 460, 280, 40 }, "SETTINGS", false, false },
    { { 500, 510, 280, 40 }, "EXIT", false, false }
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
            DrawCircleV(game->particles[i].position, game->particles[i].size, 
                        Fade(game->particles[i].color, game->particles[i].lifeTime / game->particles[i].maxLifeTime));
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

    // Desenha Botões do Menu
    for (int i = 0; i < 6; i++)
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
    DrawTextEx(font, "- Ataque circular (Slash Imune)", (Vector2){ 580, (float)(startY + spacing) }, 22.0f, 1.0f, WHITE);

    DrawTextEx(font, "ESC", (Vector2){ 280, (float)(startY + spacing * 2) }, 22.0f, 1.0f, YELLOW);
    DrawTextEx(font, "- Pausar / Voltar ao Menu", (Vector2){ 580, (float)(startY + spacing * 2) }, 22.0f, 1.0f, WHITE);

    DrawTextEx(font, "SALVAR & CARREGAR", (Vector2){ 280, (float)(startY + spacing * 3.5f) }, 22.0f, 1.0f, YELLOW);
    DrawTextEx(font, "- Use as opções de pause (ESC) para gerenciar slots", (Vector2){ 580, (float)(startY + spacing * 3.5f) }, 22.0f, 1.0f, WHITE);

    // Legenda de Itêns
    DrawTextEx(font, "POWER-UPS (AMARELOS NO MAPA):", (Vector2){ 280, (float)(startY + spacing * 5.5) }, 20.0f, 1.0f, GOLD);
    DrawTextEx(font, "Cura (+35 HP) | Velocidade (+60%) | Escudo (invencível) | Dano Duplo (x2)", (Vector2){ 280, (float)(startY + spacing * 6.5) }, 18.0f, 1.0f, LIGHTGRAY);

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

    DrawTextEx(font, TextFormat("Pontução Final: %d", game->player.score), (Vector2){ 440, 225 }, 22.0f, 1.0f, WHITE);
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
            DrawCircleV(game->particles[i].position, game->particles[i].size, 
                        Fade(game->particles[i].color, game->particles[i].lifeTime / game->particles[i].maxLifeTime));
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

    DrawTextEx(font, TextFormat("Pontução Final: %d", game->player.score), (Vector2){ 440, 225 }, 22.0f, 1.0f, GOLD);
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
static void DrawSciFiBox(Rectangle r, Color col)
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

void DrawHUD(GameState *game, Font font)
{
    // ------------------------------------------------------------------------
    // A. BARRA DE STATUS DO JOGADOR (HP & XP)
    // ------------------------------------------------------------------------
    // Painel superior esquerdo minimalista estilo cockpit espacial (360x95)
    DrawSciFiBox((Rectangle){ 20, 20, 360, 95 }, THEME_COLOR_MAIN);

    // Informações básicas (Nome à esquerda, Nível à direita)
    DrawTextEx(font, game->player.name, (Vector2){ 35, 28 }, 18.0f, 1.0f, THEME_COLOR_MAIN);
    
    char lvlStr[16];
    sprintf(lvlStr, "LV. %d", game->player.level);
    Vector2 lvlSize = MeasureTextEx(font, lvlStr, 14.0f, 1.0f);
    DrawTextEx(font, lvlStr, (Vector2){ 365.0f - lvlSize.x, 30.0f }, 14.0f, 1.0f, GOLD);

    // Barra de HP (Fundo escuro, preenchimento neon laser cyan/red)
    DrawRectangleRounded((Rectangle){ 35, 54, 220, 10 }, 0.5f, 4, (Color){ 45, 10, 15, 255 });
    float hpPercent = (float)game->player.hp / game->player.maxHp;
    if (hpPercent > 0.0f)
    {
        Color hpColor = (hpPercent > 0.45f) ? THEME_COLOR_MAIN : (hpPercent > 0.2f) ? ORANGE : RED;
        DrawRectangleRounded((Rectangle){ 35, 54, 220.0f * hpPercent, 10 }, 0.5f, 4, hpColor);
    }
    
    char hpStr[32];
    sprintf(hpStr, "%d/%d HP", game->player.hp, game->player.maxHp);
    Vector2 hpSize = MeasureTextEx(font, hpStr, 13.0f, 1.0f);
    DrawTextEx(font, hpStr, (Vector2){ 365.0f - hpSize.x, 52.0f }, 13.0f, 1.0f, WHITE);

    // Barra de XP (Laser violeta bem fino)
    DrawTextEx(font, "XP", (Vector2){ 35, 73 }, 11.0f, 1.0f, (Color){ 224, 64, 251, 255 });
    DrawRectangleRounded((Rectangle){ 60, 75, 305, 6 }, 0.5f, 4, BLACK);
    float xpPercent = (float)game->player.xp / game->player.xpNeeded;
    if (xpPercent > 0.0f)
    {
        DrawRectangleRounded((Rectangle){ 60, 75, 305.0f * xpPercent, 6 }, 0.5f, 4, (Color){ 224, 64, 251, 255 });
    }

    // ------------------------------------------------------------------------
    // B. PAINEL DE ONDA / HORDA (SUPERIOR CENTRAL - BANNER HOLO EM CAIXA SCI-FI)
    // ------------------------------------------------------------------------
    Rectangle waveBox = { 490, 20, 300, 60 };
    DrawSciFiBox(waveBox, THEME_COLOR_MAIN);

    const char *waveTxt = TextFormat("ONDA DE INFEÇÃO: %d / 5", game->wave);
    Vector2 waveTxtSize = MeasureTextEx(font, waveTxt, 16.0f, 1.0f);
    DrawTextEx(font, waveTxt, (Vector2){ 640.0f - waveTxtSize.x / 2.0f, 28.0f }, 16.0f, 1.0f, GOLD);

    const char *remTxt = TextFormat("Patógenos Ativos: %d", game->enemiesRemaining);
    Vector2 remTxtSize = MeasureTextEx(font, remTxt, 13.0f, 1.0f);
    DrawTextEx(font, remTxt, (Vector2){ 640.0f - remTxtSize.x / 2.0f, 48.0f }, 13.0f, 1.0f, WHITE);

    // ------------------------------------------------------------------------
    // C. PONTUAÇÃO (SUPERIOR DIREITO)
    // ------------------------------------------------------------------------
    DrawSciFiBox((Rectangle){ 900, 20, 150, 55 }, THEME_COLOR_MAIN);
    DrawTextEx(font, "SCORE", (Vector2){ 915, 27 }, 12.0f, 1.0f, GRAY);
    DrawTextEx(font, TextFormat("%06d", game->player.score), (Vector2){ 915, 42 }, 20.0f, 1.0f, YELLOW);

    // ------------------------------------------------------------------------
    // D. INDICADORES VISUAIS DE BUFFS ATIVOS (ABAIXO DO PAINEL DE STATUS)
    // ------------------------------------------------------------------------
    int buffCount = 0;
    
    // Buff Velocidade
    if (game->player.speedTimer > 0.0f)
    {
        Rectangle rBuff = { 20, 130.0f + (float)buffCount * 32.0f, 190, 26 };
        DrawRectangleRec(rBuff, Fade((Color){ 10, 8, 22, 255 }, 0.75f));
        DrawRectangle((int)rBuff.x, (int)rBuff.y, 4, (int)rBuff.height, YELLOW);
        DrawRectangleLinesEx(rBuff, 1.0f, Fade(YELLOW, 0.25f));
        
        DrawTextEx(font, TextFormat("SPEED: %.1fs", game->player.speedTimer), 
                   (Vector2){ rBuff.x + 12, rBuff.y + 7 }, 12.0f, 1.0f, YELLOW);
        buffCount++;
    }

    // Buff Escudo
    if (game->player.shieldTimer > 0.0f)
    {
        Rectangle rBuff = { 20, 130.0f + (float)buffCount * 32.0f, 190, 26 };
        DrawRectangleRec(rBuff, Fade((Color){ 10, 8, 22, 255 }, 0.75f));
        DrawRectangle((int)rBuff.x, (int)rBuff.y, 4, (int)rBuff.height, SKYBLUE);
        DrawRectangleLinesEx(rBuff, 1.0f, Fade(SKYBLUE, 0.25f));
        
        DrawTextEx(font, TextFormat("SHIELD: %.1fs", game->player.shieldTimer), 
                   (Vector2){ rBuff.x + 12, rBuff.y + 7 }, 12.0f, 1.0f, SKYBLUE);
        buffCount++;
    }

    // Buff Dano
    if (game->player.attackBoostTimer > 0.0f)
    {
        Rectangle rBuff = { 20, 130.0f + (float)buffCount * 32.0f, 190, 26 };
        DrawRectangleRec(rBuff, Fade((Color){ 10, 8, 22, 255 }, 0.75f));
        DrawRectangle((int)rBuff.x, (int)rBuff.y, 4, (int)rBuff.height, ORANGE);
        DrawRectangleLinesEx(rBuff, 1.0f, Fade(ORANGE, 0.25f));
        
        DrawTextEx(font, TextFormat("DAMAGE x2: %.1fs", game->player.attackBoostTimer), 
                   (Vector2){ rBuff.x + 12, rBuff.y + 7 }, 12.0f, 1.0f, ORANGE);
        buffCount++;
    }

    // ------------------------------------------------------------------------
    // E. RADAR ESPACIAL DENTRO DO HUD (CIRCULAR, CENTRADO NO PLAYER)
    // ------------------------------------------------------------------------
    Vector2 radarCenter = { 1195.0f, 85.0f };
    float radarRadius = 65.0f;
    float radarRange = 1200.0f; // Alcance do radar local
    
    // Fundo do Radar
    DrawCircleV(radarCenter, radarRadius, Fade((Color){ 10, 8, 22, 255 }, 0.65f));
    
    // Moldura decorativa sci-fi ao redor do radar
    Rectangle radarFrame = { radarCenter.x - radarRadius - 5, radarCenter.y - radarRadius - 5, radarRadius * 2 + 10, radarRadius * 2 + 10 };
    DrawRectangleLinesEx(radarFrame, 1.0f, Fade(THEME_COLOR_MAIN, 0.3f));
    // Moldura decorativa nos cantos
    float len = 6.0f;
    DrawLineEx((Vector2){ radarFrame.x, radarFrame.y }, (Vector2){ radarFrame.x + len, radarFrame.y }, 1.5f, THEME_COLOR_MAIN);
    DrawLineEx((Vector2){ radarFrame.x, radarFrame.y }, (Vector2){ radarFrame.x, radarFrame.y + len }, 1.5f, THEME_COLOR_MAIN);
    DrawLineEx((Vector2){ radarFrame.x + radarFrame.width, radarFrame.y }, (Vector2){ radarFrame.x + radarFrame.width - len, radarFrame.y }, 1.5f, THEME_COLOR_MAIN);
    DrawLineEx((Vector2){ radarFrame.x + radarFrame.width, radarFrame.y }, (Vector2){ radarFrame.x + radarFrame.width, radarFrame.y + len }, 1.5f, THEME_COLOR_MAIN);
    DrawLineEx((Vector2){ radarFrame.x, radarFrame.y + radarFrame.height }, (Vector2){ radarFrame.x + len, radarFrame.y + radarFrame.height }, 1.5f, THEME_COLOR_MAIN);
    DrawLineEx((Vector2){ radarFrame.x, radarFrame.y + radarFrame.height }, (Vector2){ radarFrame.x, radarFrame.y + radarFrame.height - len }, 1.5f, THEME_COLOR_MAIN);
    DrawLineEx((Vector2){ radarFrame.x + radarFrame.width, radarFrame.y + radarFrame.height }, (Vector2){ radarFrame.x + radarFrame.width - len, radarFrame.y + radarFrame.height }, 1.5f, THEME_COLOR_MAIN);
    DrawLineEx((Vector2){ radarFrame.x + radarFrame.width, radarFrame.y + radarFrame.height }, (Vector2){ radarFrame.x + radarFrame.width, radarFrame.y + radarFrame.height - len }, 1.5f, THEME_COLOR_MAIN);

    // Anéis Concêntricos Neon
    DrawCircleLines(radarCenter.x, radarCenter.y, radarRadius, THEME_COLOR_MAIN);
    DrawCircleLines(radarCenter.x, radarCenter.y, radarRadius * 0.66f, Fade(THEME_COLOR_MAIN, 0.25f));
    DrawCircleLines(radarCenter.x, radarCenter.y, radarRadius * 0.33f, Fade(THEME_COLOR_MAIN, 0.15f));
    
    // Retículos do Radar
    DrawLineV((Vector2){ radarCenter.x - radarRadius, radarCenter.y }, (Vector2){ radarCenter.x + radarRadius, radarCenter.y }, Fade(THEME_COLOR_MAIN, 0.2f));
    DrawLineV((Vector2){ radarCenter.x, radarCenter.y - radarRadius }, (Vector2){ radarCenter.x, radarCenter.y + radarRadius }, Fade(THEME_COLOR_MAIN, 0.2f));
    
    // Efeito sweep (varredura laser giratória)
    float sweepTime = (float)GetTime() * 3.0f;
    Vector2 sweepEnd = {
        radarCenter.x + cosf(sweepTime) * radarRadius,
        radarCenter.y + sinf(sweepTime) * radarRadius
    };
    DrawLineEx(radarCenter, sweepEnd, 1.5f, Fade(THEME_COLOR_MAIN, 0.5f));

    // A. Desenha os Power-Ups no radar (Pontos amarelos)
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (game->powerUps[i].active)
        {
            Vector2 diff = Vector2Subtract(game->powerUps[i].position, game->player.position);
            float dist = Vector2Length(diff);
            if (dist <= radarRange)
            {
                float scale = radarRadius / radarRange;
                Vector2 dotPos = Vector2Add(radarCenter, Vector2Scale(diff, scale));
                DrawCircleV(dotPos, 2.5f, YELLOW);
            }
        }
    }

    // B. Desenha os Inimigos no radar (Pontos vermelhos/laranjas/boss maior)
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (game->enemies[i].active)
        {
            Vector2 diff = Vector2Subtract(game->enemies[i].position, game->player.position);
            float dist = Vector2Length(diff);
            if (dist <= radarRange)
            {
                float scale = radarRadius / radarRange;
                Vector2 dotPos = Vector2Add(radarCenter, Vector2Scale(diff, scale));
                Color dotCol = (game->enemies[i].type == 2) ? MAROON : (game->enemies[i].state == AGGRO) ? RED : ORANGE;
                float dotSize = (game->enemies[i].type == 2) ? 3.5f : 2.0f;
                DrawCircleV(dotPos, dotSize, dotCol);
            }
        }
    }

    // C. Desenha o Jogador no centro do radar (Ponto azul ciano pulsando)
    float pPulse = 3.0f + sinf((float)GetTime() * 6.0f) * 0.8f;
    DrawCircleV(radarCenter, pPulse, SKYBLUE);
    DrawCircleLines(radarCenter.x, radarCenter.y, pPulse + 2.0f, Fade(SKYBLUE, 0.5f));

    // Rótulo do Radar
    DrawTextEx(font, "BIOSSENSOR", (Vector2){ radarCenter.x - 34.0f, radarCenter.y + radarRadius + 8.0f }, 11.0f, 1.0f, GRAY);
    
    // Notificação de salvamento no rodapé
    if (game->saveLoaded)
    {
        DrawRectangleRounded((Rectangle){ 490, 670, 300, 30 }, 0.4f, 4, Fade(GREEN, 0.2f));
        DrawRectangleRoundedLines((Rectangle){ 490, 670, 300, 30 }, 0.4f, 4, GREEN);
        
        Vector2 textSz = MeasureTextEx(font, game->notificationMsg, 14.0f, 1.0f);
        DrawTextEx(font, game->notificationMsg, (Vector2){ 490.0f + 150.0f - textSz.x/2.0f, 678.0f }, 14.0f, 1.0f, GREEN);
        
        // Some após 3 segundos
        if (game->timeElapsed > 3.0f) game->saveLoaded = false;
    }
}

// ============================================================================

// ============================================================================
// AUXILIAR: DESENHAR O JOGADOR DE ACORDO COM A SKIN ATUAL
// ============================================================================
void DrawPlayerHero(GameState *game, Vector2 pPos, float playerSize)
{
    bool isBoosted = (game->player.attackBoostTimer > 0.0f);
    Color pCol = isBoosted ? GOLD : THEME_COLOR_MAIN;
    // Renderiza a forma base do Herói (Cavaleiro Branco)
    // Passa o tamanho maior (60.0f em vez de playerSize) para mais detalhes
    DrawPlayerModel(&game->player, 60.0f, pCol, GetTime());

    // D. Efeito do Escudo (Circulo ciano translúcido ao redor)
    if (game->player.shieldTimer > 0.0f)
    {
        float ringRad = playerSize * 0.95f + sinf((float)GetTime() * 12.0f) * 3.0f;
        DrawCircleLines(pPos.x, pPos.y, ringRad, Fade(SKYBLUE, 0.85f));
        DrawCircle(pPos.x, pPos.y, ringRad, Fade(SKYBLUE, 0.12f));
    }
}

// ============================================================================
// DIBUJA A TELA DE GAMEPLAY (MUNDO 2D + HUD + RADAR)
// ============================================================================
void DrawTelaGameplay(GameState *game, Font font, bool drawHUD)
{
    // Determina a cor de fundo (Normal ou fase do Boss Superbactéria)
    Color bgColor = (Color){ 6, 14, 8, 255 };  // Verde-petóleo biológico padrão
    Color gridColor = Fade((Color){0, 100, 40, 255}, 0.15f);
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active && game->enemies[i].tier == TIER_3_BOSS) {
            float hpPercent = (float)game->enemies[i].hp / game->enemies[i].maxHp;
            if (hpPercent < 0.33f) {
                // Phase 3: Vermelho escuro distorcido
                bgColor = (Color){ 30, 5, 5, 255 };
                gridColor = Fade(RED, 0.25f);
            } else if (hpPercent < 0.66f) {
                // Phase 2: Roxo corrompido
                bgColor = (Color){ 20, 5, 25, 255 };
                gridColor = Fade(MAGENTA, 0.2f);
            }
            break;
        }
    }

    // ------------------------------------------------------------------------
    // 1. DESENHA O MUNDO DO JOGO (AFETADO PELA CÂMERA 2D)
    // ------------------------------------------------------------------------
    BeginMode2D(game->camera);

    // Desenha o mapa do Organismo separado
    bool isBossFight = false;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active && game->enemies[i].tier == TIER_3_BOSS) {
            isBossFight = true;
            break;
        }
    }
    DrawMapOrganismo(font, isBossFight, bgColor, gridColor);

    // A. Desenha os Power-Ups (Quadrados amarelos com efeito glow pulsante)
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (game->powerUps[i].active)
        {
            float pulse = 24.0f + sinf(game->powerUps[i].pulseTimer * 6.0f) * 4.0f;
            Vector2 pos = game->powerUps[i].position;

            // Efeito visual de luz ao redor
            DrawCircleV(pos, pulse + 6.0f, Fade(YELLOW, 0.18f));
            DrawRectangleRec((Rectangle){ pos.x - pulse/2.0f, pos.y - pulse/2.0f, pulse, pulse }, YELLOW);
            DrawRectangleLinesEx((Rectangle){ pos.x - pulse/2.0f, pos.y - pulse/2.0f, pulse, pulse }, 2.0f, WHITE);
            
            // Ícone interno descritivo desenhado com cor escura
            const char *itemChar = "P";
            if (game->powerUps[i].type == HP_RECOVERY) itemChar = "H";
            if (game->powerUps[i].type == SPEED_BOOST) itemChar = "S";
            if (game->powerUps[i].type == SHIELD)       itemChar = "D";
            if (game->powerUps[i].type == ATTACK_BOOST) itemChar = "A";
            DrawTextEx(font, itemChar, (Vector2){ pos.x - 4.0f, pos.y - 7.0f }, 14.0f, 1.0f, BLACK);
        }
    }

    // B. Desenha os Inimigos
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (game->enemies[i].active)
        {
            Enemy *enemy = &game->enemies[i];
            
            float destSize = (enemy->tier == TIER_3_BOSS) ? 140.0f : 45.0f;
            Vector2 renderPos = enemy->position;
            
            float squashFactor = 1.0f;
            float scale = 1.0f;
            float rotation = 0.0f;
            float alpha = 1.0f;
            
            if (enemy->state == HURT)
            {
                // Efeito de tremor (shake) baseado no tempo restante de stun
                float intensity = (enemy->cooldownTimer / 0.25f) * 6.0f;
                renderPos.x += GetRandomValue(-intensity, intensity);
                renderPos.y += GetRandomValue(-intensity, intensity);
                
                // Squash Factor: oscilação rápida ao ser atingido (pulsação)
                float t = enemy->cooldownTimer / 0.25f;
                squashFactor = 1.0f + sinf(t * PI * 4.0f) * 0.18f;
            }
            else if (enemy->state == DEATH)
            {
                float deathPct = enemy->cooldownTimer / 0.5f; // vai de 1.0 a 0.0
                if (deathPct < 0.0f) deathPct = 0.0f;
                if (deathPct > 1.0f) deathPct = 1.0f;
                
                // Encolhe o tamanho gradualmente
                scale = deathPct;
                
                // Gira em torno do próprio eixo
                rotation = (1.0f - deathPct) * 360.0f;
                
                // Desvanece (fade out)
                alpha = deathPct;
            }
            
            float currentDestSize = destSize * scale;
            
            DrawEnemyModel(enemy, renderPos, currentDestSize, rotation, squashFactor, alpha);
            
            // Barra de HP individual acima do inimigo (apenas se vivo/não em animação de morte)
            if (enemy->state != DEATH)
            {
                float size = (enemy->tier == TIER_3_BOSS) ? 400.0f : 60.0f;
                float barW = size * 1.1f;
                float barH = 6.0f;
                float yOffset = (enemy->tier == TIER_3_BOSS) ? 200.0f : 50.0f;
                Rectangle rHPBg = { enemy->position.x - barW / 2.0f, enemy->position.y - yOffset, barW, barH };
                DrawRectangleRec(rHPBg, Fade(RED, 0.4f));
                
                float enemyHpPercent = (float)enemy->hp / enemy->maxHp;
                if (enemyHpPercent > 0.0f)
                {
                    Rectangle rHPFill = { rHPBg.x, rHPBg.y, barW * enemyHpPercent, barH };
                    DrawRectangleRec(rHPFill, GREEN);
                }
                DrawRectangleLinesEx(rHPBg, 1.0f, BLACK);
            }
        }
    }

    // Desenha Projéteis
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (game->projectiles[i].active)
        {
            Projectile *p = &game->projectiles[i];
            float srcSize = 12.0f;
            Color pCol = YELLOW;
            if (p->type == PROJ_ACID_ARC) pCol = LIME;
            else if (p->type == PROJ_VOID_BOLT) pCol = MAGENTA;
            else if (p->type == PROJ_BOSS_BULLET) pCol = RED;
            
            DrawCircle(p->position.x, p->position.y, srcSize, pCol);
            DrawCircleLines(p->position.x, p->position.y, srcSize, WHITE);
        }
    }

    // ------------------------------------------------------------------------
    // 4. DESENHA O JOGADOR
    // ------------------------------------------------------------------------
    Vector2 pPos = game->player.position;
    float playerSize = 40.0f;

    DrawPlayerHero(game, pPos, playerSize);

    // E. Desenha a animação de ataque (Slash circular crescendo)
    if (game->slashAnimTimer > 0.0f)
    {
        // Progresso do slash: de 0.0 a 1.0
        float t = 1.0f - (game->slashAnimTimer / 0.22f);
        float currentRadius = t * game->slashAnimRadius;
        
        // Efeito de anel em expansão
        DrawCircleLines(game->slashAnimPos.x, game->slashAnimPos.y, currentRadius, Fade(WHITE, (1.0f - t) * 0.9f));
        DrawCircleLines(game->slashAnimPos.x, game->slashAnimPos.y, currentRadius - 8.0f, Fade(SKYBLUE, (1.0f - t) * 0.7f));
        DrawCircle(game->slashAnimPos.x, game->slashAnimPos.y, currentRadius, Fade(SKYBLUE, (1.0f - t) * 0.15f));
    }

    // F. Desenha as partículas ativas do mundo
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (game->particles[i].active)
        {
            float lifePercent = game->particles[i].lifeTime / game->particles[i].maxLifeTime;
            DrawCircleV(
                game->particles[i].position,
                game->particles[i].size * (0.3f + 0.7f * lifePercent),
                Fade(game->particles[i].color, lifePercent)
            );
        }
    }

    EndMode2D();

    // ------------------------------------------------------------------------
    // 2. RENDEREIZA A INTERFACE FIXA (HUD DO JOGADOR)
    // ------------------------------------------------------------------------
    if (drawHUD)
    {
        DrawHUD(game, font);
    }
}

// ============================================================================
// 7. TELA: TUTORIAL — INTERIOR DA SERINGA DE VACINA
// ============================================================================
void DrawTelaTutorial(GameState *game, Font font)
{
    // -------------------------------------------------------------------------
    // A. RENDERIZA O MUNDO DO TUTORIAL (INTERIOR DA SERINGA) COM CÂMERA 2D
    // -------------------------------------------------------------------------
    // Fundo branco-acinzentado médico (interior de seringa plástica)
    ClearBackground((Color){ 200, 220, 230, 255 });

    BeginMode2D(game->camera);

    // Chama a renderização do mapa separado da Seringa
    DrawMapSeringa(font, game->tutorialStep, (float)GetTime());

    // -------------------------------------------------------------------------
    // B. DESENHA O ANTICORPO (JOGADOR)
    // -------------------------------------------------------------------------
    Vector2 pPos = game->player.position;
    float ps = 38.0f;
    
    DrawPlayerHero(game, pPos, ps);

    // Animação de slash se existir
    if (game->slashAnimTimer > 0.0f)
    {
        float t = 1.0f - (game->slashAnimTimer / 0.22f);
        float cr = t * game->slashAnimRadius;
        DrawCircleLines(game->slashAnimPos.x, game->slashAnimPos.y, cr, Fade(WHITE, (1.0f - t) * 0.9f));
        DrawCircle(game->slashAnimPos.x, game->slashAnimPos.y, cr, Fade(THEME_COLOR_MAIN, (1.0f - t) * 0.15f));
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // C. DESENHA A BACTÉRIA DO TUTORIAL (PASSO 1)
    // -------------------------------------------------------------------------
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (game->enemies[i].active && game->enemies[i].isTutorialEnemy)
        {
            Enemy *e = &game->enemies[i];
            float es = 32.0f;
            
            Vector2 renderPos = e->position;
            float squashX = 1.0f;
            float squashY = 1.0f;
            float scale = 1.0f;
            float rotationAngle = 0.0f;
            float alpha = 1.0f;
            
            if (e->state == HURT)
            {
                // Tremer (shake) baseado no tempo de stun
                float intensity = (e->cooldownTimer / 0.25f) * 6.0f;
                renderPos.x += GetRandomValue(-intensity, intensity);
                renderPos.y += GetRandomValue(-intensity, intensity);
                
                // Squash/Stretch oscilante
                float t = e->cooldownTimer / 0.25f;
                squashX = 1.0f + sinf(t * PI * 4.0f) * 0.15f;
                squashY = 1.0f - sinf(t * PI * 4.0f) * 0.15f;
            }
            else if (e->state == DEATH)
            {
                float deathPct = e->cooldownTimer / 0.5f;
                if (deathPct < 0.0f) deathPct = 0.0f;
                if (deathPct > 1.0f) deathPct = 1.0f;
                
                scale = deathPct;
                rotationAngle = (1.0f - deathPct) * PI * 2.0f; // rotação em radianos
                alpha = deathPct;
            }
            
            float currentEs = es * scale;
            
            // Bactéria enfraquecida: círculo verde com "flagelo" simulado
            Color bactCol = (e->state == HURT) ? WHITE : (Color){ 50, 200, 80, 255 };
            Color bactLineCol = (Color){ 20, 140, 50, 255 };
            
            bactCol = Fade(bactCol, alpha);
            bactLineCol = Fade(bactLineCol, alpha);
            
            // Desenha corpo elíptico para suportar o squash/stretch
            DrawEllipse(renderPos.x, renderPos.y, currentEs * squashX, currentEs * squashY, bactCol);
            DrawEllipseLines(renderPos.x, renderPos.y, currentEs * squashX, currentEs * squashY, bactLineCol);
            
            // Pequenos "flagelos" representando mobilidade bacteriana, rotacionados dinamicamente no espaço 2D
            float c = cosf(rotationAngle);
            float s = sinf(rotationAngle);
            
            // Flagelo esquerdo (originalmente de (-es, 0) para (-es - 15, 10))
            Vector2 f1Start = { -currentEs * squashX * c, -currentEs * squashX * s };
            Vector2 f1End   = { (-currentEs * squashX - 15.0f) * c - 10.0f * s, (-currentEs * squashX - 15.0f) * s + 10.0f * c };
            
            // Flagelo direito (originalmente de (es, 0) para (es + 15, -10))
            Vector2 f2Start = { currentEs * squashX * c, currentEs * squashX * s };
            Vector2 f2End   = { (currentEs * squashX + 15.0f) * c - (-10.0f) * s, (currentEs * squashX + 15.0f) * s + (-10.0f) * c };
            
            DrawLineV(Vector2Add(renderPos, f1Start), Vector2Add(renderPos, f1End), bactLineCol);
            DrawLineV(Vector2Add(renderPos, f2Start), Vector2Add(renderPos, f2End), bactLineCol);
            
            // Barra de HP da bactéria (apenas se não estiver morrendo)
            if (e->state != DEATH)
            {
                float barW = 70.0f;
                DrawRectangle(e->position.x - barW/2, e->position.y - es - 14, barW, 8, Fade(RED, 0.5f));
                float hpPct = (float)e->hp / e->maxHp;
                DrawRectangle(e->position.x - barW/2, e->position.y - es - 14, barW * hpPct, 8, (Color){ 50, 200, 80, 255 });
                DrawRectangleLinesEx((Rectangle){ e->position.x - barW/2, e->position.y - es - 14, barW, 8 }, 1.0f, BLACK);
            }
        }
    }

    // Partículas
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (game->particles[i].active)
        {
            float lp = game->particles[i].lifeTime / game->particles[i].maxLifeTime;
            DrawCircleV(game->particles[i].position, game->particles[i].size * (0.3f + 0.7f * lp), Fade(game->particles[i].color, lp));
        }
    }

    // Desenha Projéteis do Tutorial
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (game->projectiles[i].active)
        {
            Vector2 pPos = game->projectiles[i].position;
            DrawCircleV(pPos, 14.0f, Fade((Color){ 50, 200, 80, 255 }, 0.22f));
            DrawCircleV(pPos, 8.0f, (Color){ 50, 200, 80, 255 });
            DrawCircleV(pPos, 4.0f, WHITE);
        }
    }

    // Desenha Power-ups do Tutorial (ampolas de vacina / esferas de treino)
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (game->powerUps[i].active)
        {
            float pulse = 24.0f + sinf(game->powerUps[i].pulseTimer * 6.0f) * 4.0f;
            Vector2 pos = game->powerUps[i].position;

            // Cores biológicas e brilhos com base no tipo
            Color itemCol = YELLOW;
            const char *itemChar = "P";
            if (game->powerUps[i].type == HP_RECOVERY) { itemCol = (Color){ 50, 220, 100, 255 }; itemChar = "H"; }
            else if (game->powerUps[i].type == SPEED_BOOST) { itemCol = THEME_COLOR_MAIN; itemChar = "S"; }
            else if (game->powerUps[i].type == SHIELD) { itemCol = (Color){ 255, 180, 0, 255 }; itemChar = "D"; }
            else if (game->powerUps[i].type == ATTACK_BOOST) { itemCol = (Color){ 255, 60, 100, 255 }; itemChar = "A"; }

            DrawCircleV(pos, pulse + 6.0f, Fade(itemCol, 0.22f));
            DrawRectangleRec((Rectangle){ pos.x - pulse/2.0f, pos.y - pulse/2.0f, pulse, pulse }, itemCol);
            DrawRectangleLinesEx((Rectangle){ pos.x - pulse/2.0f, pos.y - pulse/2.0f, pulse, pulse }, 2.0f, WHITE);
            
            DrawTextEx(font, itemChar, (Vector2){ pos.x - 4.0f, pos.y - 7.0f }, 14.0f, 1.0f, BLACK);
        }
    }

    EndMode2D();

    // -------------------------------------------------------------------------
    // D. HUD DO TUTORIAL: CAIXA DE DIÁLOGO (SOBREPOSTA, SEM CÂMERA)
    // -------------------------------------------------------------------------

    // Indicador de passo (canto superior direito)
    char stepStr[32];
    sprintf(stepStr, "PASSO %d/3", game->tutorialStep + 1);
    DrawSciFiBox((Rectangle){ 1050, 20, 210, 45 }, (Color){ 0, 200, 100, 255 });
    Vector2 stepSz = MeasureTextEx(font, stepStr, 18.0f, 1.0f);
    DrawTextEx(font, stepStr, (Vector2){ 1155.0f - stepSz.x / 2.0f, 32.0f }, 18.0f, 1.0f, (Color){ 0, 220, 120, 255 });

    if (game->tutorialDialog.active)
    {
        // Painel de diálogo inferior
        DrawRectangleRounded((Rectangle){ 80, 580, 1120, 120 }, 0.08f, 6, Fade((Color){ 6, 18, 12, 255 }, 0.92f));
        DrawRectangleRoundedLines((Rectangle){ 80, 580, 1120, 120 }, 0.08f, 6, (Color){ 0, 200, 100, 255 });

        // Ícone de personagem/anticorpo no canto esquerdo do diálogo
        DrawCircle(120, 640, 22, (Color){ 0, 150, 200, 255 });
        DrawCircleLines(120, 640, 22, THEME_COLOR_MAIN);
        DrawTextEx(font, "Ab", (Vector2){ 108, 630 }, 16.0f, 1.0f, WHITE);

        const char *fullL1 = "";
        const char *fullL2 = "";
        const char *fullL3 = "";
        GetTutorialDialogText(game->tutorialStep, game->tutorialDialog.page, &fullL1, &fullL2, &fullL3);

        // Buffers para as linhas a serem desenhadas
        char l1Draw[128] = { 0 };
        char l2Draw[128] = { 0 };
        char l3Draw[128] = { 0 };

        int charLimit = game->tutorialDialog.charShown;
        int len1 = strlen(fullL1);
        int len2 = strlen(fullL2);
        int len3 = strlen(fullL3);

        // Linha 1
        if (charLimit <= len1)
        {
            strncpy(l1Draw, fullL1, charLimit);
        }
        else
        {
            strcpy(l1Draw, fullL1);
            int rem = charLimit - len1;
            // Linha 2
            if (rem <= len2)
            {
                strncpy(l2Draw, fullL2, rem);
            }
            else
            {
                strcpy(l2Draw, fullL2);
                int rem2 = rem - len2;
                // Linha 3
                if (rem2 <= len3)
                {
                    strncpy(l3Draw, fullL3, rem2);
                }
                else
                {
                    strcpy(l3Draw, fullL3);
                }
            }
        }

        DrawTextEx(font, l1Draw, (Vector2){ 155, 592 }, 16.0f, 1.0f, WHITE);
        DrawTextEx(font, l2Draw, (Vector2){ 155, 614 }, 16.0f, 1.0f, WHITE);
        DrawTextEx(font, l3Draw, (Vector2){ 155, 636 }, 16.0f, 1.0f, (Color){ 0, 220, 120, 255 });
    }

    // HP do jogador (simples, canto superior esquerdo)
    DrawSciFiBox((Rectangle){ 20, 20, 240, 55 }, (Color){ 0, 200, 100, 255 });
    DrawTextEx(font, "ANTICORPO", (Vector2){ 30, 27 }, 14.0f, 1.0f, (Color){ 0, 220, 120, 255 });
    DrawRectangleRounded((Rectangle){ 30, 46, 180, 10 }, 0.5f, 4, (Color){ 30, 10, 10, 255 });
    float hpPctPlayer = (float)game->player.hp / game->player.maxHp;
    if (hpPctPlayer > 0.0f)
        DrawRectangleRounded((Rectangle){ 30, 46, 180.0f * hpPctPlayer, 10 }, 0.5f, 4, (Color){ 0, 220, 120, 255 });
    char hpBuf[24];
    sprintf(hpBuf, "%d/%d HP", game->player.hp, game->player.maxHp);
    Vector2 hpSz = MeasureTextEx(font, hpBuf, 12.0f, 1.0f);
    DrawTextEx(font, hpBuf, (Vector2){ 220.0f - hpSz.x, 44.0f }, 12.0f, 1.0f, WHITE);
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
    
    DrawTextEx(font, "MASTER VOLUME", (Vector2){ 380, 250 }, 24.0f, 1.0f, WHITE);
    
    Rectangle sliderBg = { 600, 250, 300, 20 };
    Rectangle sliderFill = { 600, 250, 300 * game->masterVolume, 20 };
    
    DrawRectangleRec(sliderBg, Fade(BLACK, 0.6f));
    DrawRectangleRec(sliderFill, THEME_COLOR_MAIN);
    DrawRectangleLinesEx(sliderBg, 2.0f, THEME_COLOR_BORDER);
    
    char volText[16];
    sprintf(volText, "%d%%", (int)(game->masterVolume * 100));
    DrawTextEx(font, volText, (Vector2){ 920, 248 }, 24.0f, 1.0f, WHITE);

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
    // Fundo verde-petróleo biológico escuro degradê
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                           (Color){ 8, 20, 12, 255 }, (Color){ 10, 28, 20, 255 });

    // Células/partículas decorativas de fundo que simulam o sangue/tecido
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (game->particles[i].active)
        {
            DrawCircleV(game->particles[i].position, game->particles[i].size,
                        Fade(game->particles[i].color, game->particles[i].lifeTime / game->particles[i].maxLifeTime * 0.15f));
        }
    }

    // Título
    const char *titulo = "CARREGANDO SISTEMA IMUNOLOGICO...";
    Vector2 titleSz = MeasureTextEx(font, titulo, 26.0f, 1.2f);
    DrawTextEx(font, titulo, (Vector2){ (SCREEN_WIDTH / 2.0f) - (titleSz.x / 2.0f), 240.0f }, 26.0f, 1.2f, THEME_COLOR_MAIN);

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
}
