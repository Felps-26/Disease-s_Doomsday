// telas.c
// Implementação do HUD, minimapa, efeitos visuais e menus interativos.
#include "../../include/telas.h"
#include "../../include/gameplay.h"
#include "../../include/input_controller.h"
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
// Menu principal: opções claras para todos os sistemas do jogo.
UIButton menuButtons[] = {
    { { 500, 248, 280, 34 }, "JOGAR", false, false },
    { { 500, 286, 280, 34 }, "CARREGAR JOGO", false, false },
    { { 500, 324, 280, 34 }, "ARSENAL (ARMAS)", false, false },
    { { 500, 362, 280, 34 }, "SKINS", false, false },
    { { 500, 400, 280, 34 }, "TUTORIAL", false, false },
    { { 500, 438, 280, 34 }, "CONFIG", false, false },
    { { 500, 476, 280, 34 }, "MODO ADMIN", false, false },
    { { 500, 514, 280, 34 }, "SAIR", false, false }
};
#define MENU_BTN_COUNT ((int)(sizeof(menuButtons) / sizeof(menuButtons[0])))

// Retângulos do seletor de dificuldade no menu (3 segmentos), abaixo do painel.
Rectangle MenuDifficultyRect(int i)
{
    float segW = 200.0f, gap = 12.0f;
    float total = 3 * segW + 2 * gap;
    float startX = (SCREEN_WIDTH - total) / 2.0f;
    return (Rectangle){ startX + i * (segW + gap), 632.0f, segW, 46.0f };
}

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
// AUXILIAR: TEXTO CENTRADO QUE SE AJUSTA À LARGURA (com sombra opcional)
// Reduz a fonte ate caber em (maxW) para nunca estourar/sobrepor componentes.
// ============================================================================
void DrawTextFitCentered(Font font, const char *text, Rectangle area, float maxFont, Color color, bool shadow)
{
    float fs = maxFont;
    float padding = 16.0f;
    Vector2 sz = MeasureTextEx(font, text, fs, 1.0f);
    while (sz.x > area.width - padding && fs > 9.0f)
    {
        fs -= 1.0f;
        sz = MeasureTextEx(font, text, fs, 1.0f);
    }
    Vector2 pos = { area.x + (area.width - sz.x) / 2.0f, area.y + (area.height - sz.y) / 2.0f };
    if (shadow)
        DrawTextEx(font, text, (Vector2){ pos.x + 1.5f, pos.y + 1.5f }, fs, 1.0f, Fade(BLACK, 0.55f));
    DrawTextEx(font, text, pos, fs, 1.0f, color);
}

// ============================================================================
// AUXILIAR: BOTÃO ESTILIZADO COM HOVER/PRESS ANIMADO E TEXTO QUE SE AJUSTA
// - hover: leve escala + brilho/glow + borda pulsante ciano;
// - pressionado: compressão (escala < 1) para feedback de clique;
// - desabilitado: cinza apagado e claramente inativo;
// - texto encolhe automaticamente para nunca sair do botão.
// ============================================================================
void DrawButton(UIButton botao, Font font, bool enabled)
{
    extern Vector2 g_virtualMouse;
    bool pressed = enabled && botao.hover && IsMouseButtonDown(MOUSE_LEFT_BUTTON) &&
                   CheckCollisionPointRec(g_virtualMouse, botao.bounds);

    // Escala animada (hover cresce, pressionado comprime)
    float sc = !enabled ? 1.0f : pressed ? 0.96f : (botao.hover ? 1.04f : 1.0f);
    Rectangle b = botao.bounds;
    float cx = b.x + b.width / 2.0f, cy = b.y + b.height / 2.0f;
    Rectangle r = { cx - b.width * sc / 2.0f, cy - b.height * sc / 2.0f, b.width * sc, b.height * sc };

    Color corFundo = Fade((Color){ 26, 21, 44, 255 }, 0.78f);
    Color corBorda = (Color){ 104, 76, 172, 255 };
    Color corTexto = WHITE;

    if (!enabled)
    {
        corFundo = Fade((Color){ 15, 12, 24, 255 }, 0.5f);
        corBorda = Fade(GRAY, 0.25f);
        corTexto = Fade(GRAY, 0.4f);
    }
    else if (pressed)
    {
        corFundo = (Color){ 120, 80, 220, 255 };
        corBorda = THEME_COLOR_MAIN;
        corTexto = WHITE;
    }
    else if (botao.hover)
    {
        // Glow externo pulsante ao passar o mouse
        float pulse = 0.45f + 0.25f * sinf((float)GetTime() * 6.0f);
        DrawRectangleRounded((Rectangle){ r.x - 5, r.y - 5, r.width + 10, r.height + 10 }, 0.3f, 8, Fade(THEME_COLOR_MAIN, 0.12f * pulse + 0.05f));
        corFundo = Fade(THEME_COLOR_BORDER, 0.85f);
        corBorda = THEME_COLOR_MAIN;
        corTexto = THEME_COLOR_MAIN;
    }

    // Fundo e borda
    DrawRectangleRounded(r, 0.25f, 8, corFundo);
    DrawRectangleRoundedLines(r, 0.25f, 8, corBorda);
    if (enabled && botao.hover && !pressed)
    {
        float pulse = 0.5f + 0.5f * sinf((float)GetTime() * 6.0f);
        DrawRectangleRoundedLines(r, 0.25f, 8, Fade(THEME_COLOR_MAIN, pulse));
        // Acento lateral animado
        DrawRectangle((int)r.x + 3, (int)(r.y + r.height * 0.2f), 3, (int)(r.height * 0.6f), Fade(THEME_COLOR_MAIN, pulse));
    }

    // Texto centralizado e ajustado à largura (com sombra)
    DrawTextFitCentered(font, botao.text, r, 22.0f, corTexto, true);
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
    
    Vector2 titlePos = {
        (SCREEN_WIDTH / 2.0f) - (titleSize.x / 2.0f),
        75.0f + titleOffsetY
    };

    // Glow/halo ciano pulsante atrás do título (acabamento profissional)
    float glowPulse = 0.35f + 0.15f * sinf(time * 3.0f);
    for (int gx = -2; gx <= 2; gx++)
        for (int gy = -2; gy <= 2; gy++)
            if (gx != 0 || gy != 0)
                DrawTextEx(font, titulo, (Vector2){ titlePos.x + gx * 2.0f, titlePos.y + gy * 2.0f },
                           (float)titleFontSize, 2.0f, Fade(THEME_COLOR_MAIN, 0.05f * glowPulse));

    // Sombra do título
    DrawTextEx(font, titulo, (Vector2){ titlePos.x + 3.0f, titlePos.y + 3.0f }, (float)titleFontSize, 2.0f, Fade(BLACK, 0.45f));

    // Texto principal (verde lime biológico) + leve contorno escuro
    DrawTextEx(font, titulo, (Vector2){ titlePos.x - 1.5f, titlePos.y - 1.5f }, (float)titleFontSize, 2.0f, Fade((Color){ 0, 60, 40, 255 }, 0.6f));
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

    // Desenha Botões do Menu (índice 1 = Carregar, desabilitado sem saves)
    for (int i = 0; i < MENU_BTN_COUNT; i++)
    {
        DrawButton(menuButtons[i], font, (i == 1) ? anySaveExists : true);
    }

    // Rodapé decorativo no painel
    DrawLineEx((Vector2){ 460, 556 }, (Vector2){ 820, 556 }, 1.0f, Fade(THEME_COLOR_TEXT, 0.25f));
    const char *verText = "Armas: 1-4  |  Skins  |  5 hordas + CHEFE na onda 5";
    Vector2 verSize = MeasureTextEx(font, verText, 12.0f, 1.0f);
    DrawTextEx(font, verText, (Vector2){ 640.0f - verSize.x / 2.0f, 564.0f }, 12.0f, 1.0f, Fade(THEME_COLOR_TEXT, 0.6f));

    // ---- SELETOR DE DIFICULDADE (abaixo do painel) ----
    DrawTextEx(font, "DIFICULDADE:", (Vector2){ MenuDifficultyRect(0).x, 608.0f }, 16.0f, 1.0f, THEME_COLOR_MAIN);
    const char *dn[3] = { "FACIL", "MEDIO", "DIFICIL" };
    const char *dd[3] = { "Inimigos lentos, pouca esquiva", "Equilibrado e padrao", "Rapidos, espertos, mais esquiva" };
    Color dc[3] = { (Color){ 120, 220, 140, 255 }, (Color){ 0, 229, 255, 255 }, (Color){ 255, 90, 90, 255 } };
    for (int i = 0; i < 3; i++)
    {
        Rectangle r = MenuDifficultyRect(i);
        bool active = (game->difficulty == i);
        bool hover = CheckCollisionPointRec(g_virtualMouse, r);
        DrawRectangleRounded(r, 0.25f, 6, active ? Fade(dc[i], 0.25f) : Fade((Color){ 12, 14, 22, 255 }, 0.85f));
        DrawRectangleRoundedLines(r, 0.25f, 6, active ? dc[i] : (hover ? YELLOW : THEME_COLOR_BORDER));
        Vector2 ns = MeasureTextEx(font, dn[i], 20.0f, 1.0f);
        DrawTextEx(font, dn[i], (Vector2){ r.x + r.width / 2.0f - ns.x / 2.0f, r.y + 4.0f }, 20.0f, 1.0f, active ? dc[i] : WHITE);
        Vector2 ds = MeasureTextEx(font, dd[i], 11.0f, 1.0f);
        DrawTextEx(font, dd[i], (Vector2){ r.x + r.width / 2.0f - ds.x / 2.0f, r.y + 28.0f }, 11.0f, 1.0f, Fade(WHITE, 0.7f));
    }

    // Rodapé
    DrawTextEx(font, "ESC pausa | Dificuldade ativa: ", (Vector2){ 20, SCREEN_HEIGHT - 24 }, 14.0f, 1.0f, DARKGRAY);
    DrawTextEx(font, DifficultyName(game->difficulty), (Vector2){ 270, SCREEN_HEIGHT - 24 }, 14.0f, 1.0f, dc[game->difficulty % 3]);
}



// ============================================================================
// 2. TELA: TUTORIAL (GUIA EM ABAS) — acessível pelo Menu
// ============================================================================
#define TUT_TAB_COUNT 7
static int g_tutTab = 0;
static const char *g_tutTabNames[TUT_TAB_COUNT] = {
    "BASICO", "ARMAS", "INIMIGOS", "CHEFE", "SKINS", "XP/UPGRADES", "HORDAS"
};

static Rectangle TutTabRect(int i)
{
    float tabW = 150.0f, gap = 8.0f;
    float total = TUT_TAB_COUNT * tabW + (TUT_TAB_COUNT - 1) * gap;
    float startX = (SCREEN_WIDTH - total) / 2.0f;
    return (Rectangle){ startX + i * (tabW + gap), 116.0f, tabW, 40.0f };
}

// Linha "rótulo: texto" auxiliar do tutorial
static void TutLine(Font font, float x, float y, const char *label, Color lc, const char *text)
{
    DrawTextEx(font, label, (Vector2){ x, y }, 18.0f, 1.0f, lc);
    if (text && text[0])
    {
        float lw = MeasureTextEx(font, label, 18.0f, 1.0f).x;
        DrawTextEx(font, text, (Vector2){ x + lw + 10.0f, y }, 18.0f, 1.0f, Fade(WHITE, 0.85f));
    }
}

static void DrawTutContent(GameState *game, Font font, Rectangle panel)
{
    float x = panel.x + 28.0f;
    float y = panel.y + 22.0f;
    float step = 30.0f;

    switch (g_tutTab)
    {
        case 0: // BASICO + CONTROLES
            DrawTextEx(font, "OBJETIVO", (Vector2){ x, y }, 22.0f, 1.0f, THEME_COLOR_MAIN); y += 30.0f;
            DrawTextEx(font, "Voce e um Anticorpo. Sobreviva a 5 hordas de patogenos, derrote o", (Vector2){ x, y }, 17.0f, 1.0f, Fade(WHITE, 0.85f)); y += 24.0f;
            DrawTextEx(font, "chefe da onda 5 e proteja o organismo do Distrito Federal.", (Vector2){ x, y }, 17.0f, 1.0f, Fade(WHITE, 0.85f)); y += 38.0f;
            DrawTextEx(font, "CONTROLES", (Vector2){ x, y }, 22.0f, 1.0f, THEME_COLOR_MAIN); y += 32.0f;
            TutLine(font, x, y, "WASD / SETAS", YELLOW, "Mover o Anticorpo"); y += step;
            TutLine(font, x, y, "CLIQUE ESQ. / ESPACO", YELLOW, "Atacar com a arma equipada"); y += step;
            TutLine(font, x, y, "1  2  3  4", YELLOW, "Selecionar / trocar de arma"); y += step;
            TutLine(font, x, y, "E", YELLOW, "Usar pocao de vida (cura 50%)"); y += step;
            TutLine(font, x, y, "ESC", YELLOW, "Pausar / voltar"); y += step;
            TutLine(font, x, y, "F5 / F9", YELLOW, "Salvar / carregar rapido (slot 1)"); y += step;
            TutLine(font, x, y, "POWER-UPS", GOLD, "Cura, Velocidade, Escudo e Dano x2 (brilham no mapa)");
            break;

        case 1: // ARMAS
            DrawTextEx(font, "Troque a qualquer momento com 1-4. Cada arma muda a jogabilidade:", (Vector2){ x, y }, 17.0f, 1.0f, Fade(WHITE, 0.85f));
            y += 36.0f;
            for (int w = 1; w <= 4; w++)
            {
                WeaponInfo wi = GetWeaponInfo(w);
                DrawTextEx(font, TextFormat("[%d] %s", wi.key, wi.name), (Vector2){ x, y }, 20.0f, 1.0f, wi.color); y += 24.0f;
                DrawTextEx(font, TextFormat("Dano %d (+ATK) | %s | cooldown %.2fs | %s",
                           wi.baseDamage, wi.speedTxt, wi.cooldown, wi.special),
                           (Vector2){ x + 16, y }, 15.0f, 1.0f, Fade(WHITE, 0.8f)); y += 22.0f;
                DrawTextEx(font, TextFormat("Desbloqueio: Nivel %d  -  %s", wi.unlockLevel, wi.playstyle),
                           (Vector2){ x + 16, y }, 14.0f, 1.0f, Fade((Color){ 120, 220, 140, 255 }, 0.9f)); y += 28.0f;
            }
            break;

        case 2: // INIMIGOS
        {
            const char *en[5][3] = {
                { "SARS-CoV-2 (verde)", "Melee equilibrado, cerca voce em grupo.", "Use a Lamina em 360 e o empurrao." },
                { "Dengue / Aedes (cinza)", "Rapido, atira em leque e foge se voce chega perto.", "Persiga com o Fuzil; nao deixe atirar a vontade." },
                { "KPC Superbacteria (vermelha)", "Lenta, MUITA vida, tiro pesado. Vira CHEFE na onda 5.", "Veneno da Granada e a Vacina BFG." },
                { "Trypanosoma / Chagas (roxa)", "Muito rapida e fragil; foge com pouca vida.", "Um golpe forte resolve; nao a deixe te cercar." },
                { "Tuberculose / TB (escura)", "Atirador pesado, lento e resistente.", "Mantenha distancia e foque fogo (BFG perfura)." },
            };
            for (int i = 0; i < 5; i++)
            {
                DrawTextEx(font, en[i][0], (Vector2){ x, y }, 19.0f, 1.0f, THEME_COLOR_MAIN); y += 24.0f;
                DrawTextEx(font, en[i][1], (Vector2){ x + 16, y }, 15.0f, 1.0f, Fade(WHITE, 0.82f)); y += 21.0f;
                TutLine(font, x + 16, y, "Dica:", GOLD, en[i][2]); y += 28.0f;
            }
            break;
        }

        case 3: // CHEFE
            DrawTextEx(font, "CHEFE: SUPERBACTERIA KPC", (Vector2){ x, y }, 24.0f, 1.0f, (Color){ 255, 80, 90, 255 }); y += 34.0f;
            DrawTextEx(font, "Aparece SEMPRE na onda 5, com o corpo bem maior e milhares de vida.", (Vector2){ x, y }, 17.0f, 1.0f, Fade(WHITE, 0.85f)); y += 26.0f;
            DrawTextEx(font, "Surge CERCADO por lacaios que o escoltam e te distraem.", (Vector2){ x, y }, 17.0f, 1.0f, Fade(WHITE, 0.85f)); y += 26.0f;
            DrawTextEx(font, "Tem 3 FASES conforme perde vida:", (Vector2){ x, y }, 18.0f, 1.0f, THEME_COLOR_MAIN); y += 28.0f;
            TutLine(font, x + 16, y, "Fase 1:", GOLD, "tiros duplos em ritmo normal."); y += 26.0f;
            TutLine(font, x + 16, y, "Fase 2:", GOLD, "mais rapido e INVOCA lacaios."); y += 26.0f;
            TutLine(font, x + 16, y, "Fase 3:", GOLD, "enfurecido, dispara rajada em todas as direcoes."); y += 32.0f;
            DrawTextEx(font, "Prepare-se: suba de nivel antes, leve pocoes e guarde Granada + BFG.", (Vector2){ x, y }, 16.0f, 1.0f, (Color){ 120, 220, 140, 255 });
            break;

        case 4: // SKINS
            DrawTextEx(font, "Abra 'SKINS' no menu para escolher (preview ao vivo).", (Vector2){ x, y }, 17.0f, 1.0f, Fade(WHITE, 0.85f)); y += 34.0f;
            DrawTextEx(font, "SKINS DO ANTICORPO:", (Vector2){ x, y }, 18.0f, 1.0f, THEME_COLOR_MAIN); y += 28.0f;
            TutLine(font, x + 16, y, "PADRAO", YELLOW, "cavaleiro imune classico."); y += 26.0f;
            TutLine(font, x + 16, y, "MEDICA", YELLOW, "jaleco branco com cruz vermelha."); y += 26.0f;
            TutLine(font, x + 16, y, "INFECTADA", YELLOW, "armadura roxa corrompida."); y += 34.0f;
            DrawTextEx(font, "SKIN DA ARMA muda a cor dos disparos (Padrao, Plasma, Toxica).", (Vector2){ x, y }, 16.0f, 1.0f, Fade(WHITE, 0.8f)); y += 26.0f;
            DrawTextEx(font, "As skins sao visuais e nao alteram o balanceamento.", (Vector2){ x, y }, 15.0f, 1.0f, Fade(GRAY, 0.9f));
            break;

        case 5: // XP / UPGRADES
            DrawTextEx(font, "PROGRESSAO DE RPG", (Vector2){ x, y }, 22.0f, 1.0f, THEME_COLOR_MAIN); y += 32.0f;
            TutLine(font, x, y, "XP:", GOLD, "derrotar patogenos enche a barra roxa de XP."); y += 28.0f;
            TutLine(font, x, y, "NIVEL:", GOLD, "ao subir, ganha +Vida, +Dano, +Velocidade e cura total."); y += 28.0f;
            TutLine(font, x, y, "ARMAS:", GOLD, "Fuzil (Nv2), Granada (Nv3) e BFG (Nv4) desbloqueiam por nivel."); y += 28.0f;
            DrawTextEx(font, "PONTOS DO SUS", (Vector2){ x, y }, 20.0f, 1.0f, THEME_COLOR_MAIN); y += 28.0f;
            TutLine(font, x, y, "QUIZ:", GOLD, "acertar o quiz entre ondas da +50 Pontos do SUS."); y += 26.0f;
            TutLine(font, x, y, "LOJA:", GOLD, "gaste os pontos em +Vida Max, +Velocidade ou +Dano."); y += 26.0f;
            DrawTextEx(font, "A tela de Melhorias do SUS aparece automaticamente entre as ondas.", (Vector2){ x, y }, 15.0f, 1.0f, Fade(WHITE, 0.8f));
            break;

        default: // HORDAS
            DrawTextEx(font, "SISTEMA DE HORDAS", (Vector2){ x, y }, 22.0f, 1.0f, THEME_COLOR_MAIN); y += 32.0f;
            DrawTextEx(font, "Sao 5 ondas, cada uma mais dificil: mais inimigos e mais fortes.", (Vector2){ x, y }, 17.0f, 1.0f, Fade(WHITE, 0.85f)); y += 26.0f;
            DrawTextEx(font, "Elimine todos os patogenos da onda para avancar.", (Vector2){ x, y }, 17.0f, 1.0f, Fade(WHITE, 0.85f)); y += 26.0f;
            DrawTextEx(font, "Entre as ondas: QUIZ educativo + tela de Melhorias do SUS.", (Vector2){ x, y }, 17.0f, 1.0f, Fade(WHITE, 0.85f)); y += 32.0f;
            TutLine(font, x, y, "ONDA 5:", (Color){ 255, 90, 90, 255 }, "batalha de CHEFE garantida (veja a aba CHEFE)."); y += 30.0f;
            DrawTextEx(font, "Um banner no topo avisa cada nova horda e a chegada do chefe.", (Vector2){ x, y }, 15.0f, 1.0f, (Color){ 120, 220, 140, 255 });
            break;
    }
}

void DrawTelaControles(GameState *game, Font font)
{
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                           (Color){ 6, 14, 10, 255 }, (Color){ 10, 22, 16, 255 });

    DrawTextEx(font, "TUTORIAL", (Vector2){ SCREEN_WIDTH / 2.0f - MeasureTextEx(font, "TUTORIAL", 40.0f, 1.5f).x / 2.0f, 48.0f }, 40.0f, 1.5f, THEME_COLOR_TEXT);

    // Abas
    for (int i = 0; i < TUT_TAB_COUNT; i++)
    {
        Rectangle tr = TutTabRect(i);
        bool active = (g_tutTab == i);
        bool hover = CheckCollisionPointRec(g_virtualMouse, tr);
        DrawRectangleRounded(tr, 0.25f, 6, active ? Fade(THEME_COLOR_MAIN, 0.22f) : Fade((Color){ 12, 14, 22, 255 }, 0.85f));
        DrawRectangleRoundedLines(tr, 0.25f, 6, active ? THEME_COLOR_MAIN : (hover ? YELLOW : THEME_COLOR_BORDER));
        Vector2 nSz = MeasureTextEx(font, g_tutTabNames[i], 14.0f, 1.0f);
        DrawTextEx(font, g_tutTabNames[i], (Vector2){ tr.x + tr.width / 2.0f - nSz.x / 2.0f, tr.y + 12.0f }, 14.0f, 1.0f,
                   active ? THEME_COLOR_MAIN : Fade(WHITE, 0.85f));
    }

    // Painel de conteúdo
    Rectangle panel = { 120, 176, 1040, 432 };
    DrawRectangleRounded(panel, 0.04f, 6, Fade(BLACK, 0.5f));
    DrawRectangleRoundedLines(panel, 0.04f, 6, THEME_COLOR_BORDER);
    DrawTutContent(game, font, panel);

    DrawTextEx(font, "Use as ABAS (clique) ou SETAS <- ->   |   ESC para voltar",
               (Vector2){ 120, 620 }, 15.0f, 1.0f, Fade(WHITE, 0.6f));

    // Botão voltar
    DrawButton(controlsButton, font, true);
}

// Atualização da tela de tutorial: navegação de abas + voltar
void UpdateTelaTutorial(GameState *game, Vector2 mouse)
{
    for (int i = 0; i < TUT_TAB_COUNT; i++)
    {
        if (CheckCollisionPointRec(mouse, TutTabRect(i)) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            g_tutTab = i;
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) g_tutTab = (g_tutTab + 1) % TUT_TAB_COUNT;
    if (IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A)) g_tutTab = (g_tutTab + TUT_TAB_COUNT - 1) % TUT_TAB_COUNT;

    UpdateBtnState(&controlsButton, mouse);
    if (controlsButton.clicked || IsKeyPressed(KEY_ESCAPE))
        game->currentScreen = SCREEN_MENU;
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
    "DICA DE SAUDE: Informe a Vigilancia Ambiental sobre lotes vagos com acumulo de lixo ou agua parada na sua regiao administrativa.",
    "DICA DE SAUDE: Nao se automedique com antibioticos. O uso incorreto cria superbacterias resistentes, como a KPC, comuns em hospitais.",
    "DICA DE SAUDE: Use antibioticos apenas com prescricao e complete todo o tratamento, mesmo que os sintomas melhorem antes.",
    "DICA DE SAUDE: Mascaras e distanciamento reduzem a transmissao de doencas respiratorias como a gripe (Influenza).",
    "CURIOSIDADE: Bacteriofagos sao virus que infectam bacterias. Vacinas treinam o sistema imune contra virus de RNA, como dengue e influenza.",
    "CURIOSIDADE: O capsideo e a capa proteica que protege o material genetico do virus. Neutraliza-lo e essencial para conter a infeccao."
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

        // BUGFIX VISUAL: antes o "heroi" era uma elipse com squash extremo
        // (1.85 x 0.38), parecendo uma figura estranha esmagada. Agora e uma
        // CAPSULA/anticorpo que permanece arredondada, apenas pulsando e
        // descendo suavemente pela agulha — leitura limpa e tematica.
        float t = (float)GetTime();
        float pulse = 1.0f + sinf(t * 8.0f) * 0.05f;          // pulsacao sutil
        float capR = (26.0f - ease * 6.0f) * pulse;            // encolhe pouco ao injetar
        Color capCol = (Color){ 235, 245, 255, 255 };
        // Halo de energia
        DrawCircleV((Vector2){ cx, cy }, capR + 8.0f, Fade(THEME_COLOR_MAIN, 0.18f));
        // Corpo arredondado do anticorpo
        DrawCircleV((Vector2){ cx, cy }, capR, capCol);
        DrawCircleLines((int)cx, (int)cy, capR, THEME_COLOR_MAIN);
        // Marca em "Y" do anticorpo
        DrawLineEx((Vector2){ cx, cy + capR * 0.2f }, (Vector2){ cx, cy + capR * 0.6f }, 3.0f, Fade(THEME_COLOR_MAIN, 0.8f));
        DrawLineEx((Vector2){ cx, cy + capR * 0.2f }, (Vector2){ cx - capR * 0.45f, cy - capR * 0.4f }, 3.0f, Fade(THEME_COLOR_MAIN, 0.8f));
        DrawLineEx((Vector2){ cx, cy + capR * 0.2f }, (Vector2){ cx + capR * 0.45f, cy - capR * 0.4f }, 3.0f, Fade(THEME_COLOR_MAIN, 0.8f));

        // Gotículas de energia descendo pela agulha (sentido da injecao)
        if (ease > 0.2f)
        {
            for (int i = 0; i < 5; i++)
            {
                float dy = fmodf(t * 120.0f + i * 40.0f, 120.0f);
                DrawCircleV((Vector2){ cx, cy + gap + 10.0f + dy }, 2.5f, Fade((Color){ 0, 200, 255, 255 }, 0.5f * ease));
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
    if (tipIndex < 0 || tipIndex >= LOADING_TIP_COUNT) tipIndex = 0;
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
