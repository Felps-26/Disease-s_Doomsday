// tela_arsenal.c
// Tela de Arsenal: mostra TODAS as armas com nome, dano, cadência, cooldown,
// efeito especial, como muda a jogabilidade, tecla e requisito de desbloqueio.
#include "../../include/telas.h"
#include "../../include/gameplay.h"
#include <stdio.h>

static UIButton arsenalBack = { { 490, 650, 300, 46 }, "VOLTAR", false, false };

// Desenha um pequeno "ícone" representando cada arma (formas simples).
static void DrawWeaponGlyph(int weapon, Vector2 c, Color col)
{
    switch (weapon)
    {
        case 1: // Lâmina: losango (golpe em volta)
            DrawCircleLines((int)c.x, (int)c.y, 22.0f, Fade(col, 0.6f));
            DrawPoly(c, 4, 14.0f, 45.0f, col);
            break;
        case 2: // Fuzil: três tiros
            for (int i = 0; i < 3; i++)
                DrawRectangle((int)c.x - 18 + i * 14, (int)c.y - 3, 9, 6, col);
            break;
        case 3: // Granada: círculo com brilho (área)
            DrawCircleV(c, 20.0f, Fade(col, 0.35f));
            DrawCircleV(c, 11.0f, col);
            break;
        default: // BFG: orbe perfurante com rastro
            DrawCircleV(c, 16.0f, col);
            DrawCircleLines((int)c.x, (int)c.y, 22.0f, Fade(col, 0.7f));
            DrawLineEx((Vector2){ c.x - 26, c.y }, (Vector2){ c.x + 26, c.y }, 3.0f, Fade(col, 0.6f));
            break;
    }
}

void DrawTelaArsenal(GameState *game, Font font)
{
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, THEME_COLOR_BG_DARK, THEME_COLOR_BG_LIGHT);

    const char *title = "ARSENAL DO ANTICORPO";
    Vector2 tSz = MeasureTextEx(font, title, 40.0f, 1.5f);
    DrawTextEx(font, title, (Vector2){ SCREEN_WIDTH / 2.0f - tSz.x / 2.0f, 36.0f }, 40.0f, 1.5f, THEME_COLOR_TEXT);
    const char *sub = "Troque de arma a qualquer momento com as teclas 1, 2, 3 e 4.";
    Vector2 sSz = MeasureTextEx(font, sub, 16.0f, 1.0f);
    DrawTextEx(font, sub, (Vector2){ SCREEN_WIDTH / 2.0f - sSz.x / 2.0f, 84.0f }, 16.0f, 1.0f, Fade(WHITE, 0.8f));

    float cardW = 540.0f, cardH = 222.0f, gapX = 30.0f, gapY = 24.0f;
    float startX = (SCREEN_WIDTH - (2 * cardW + gapX)) / 2.0f;
    float startY = 120.0f;

    for (int s = 0; s < 4; s++)
    {
        WeaponInfo wi = GetWeaponInfo(s + 1);
        int col = s % 2, row = s / 2;
        Rectangle card = { startX + col * (cardW + gapX), startY + row * (cardH + gapY), cardW, cardH };

        bool unlocked = (game->player.level >= wi.unlockLevel);

        DrawRectangleRounded(card, 0.06f, 8, Fade((Color){ 10, 14, 20, 255 }, 0.85f));
        DrawRectangleRoundedLines(card, 0.06f, 8, Fade(wi.color, unlocked ? 0.9f : 0.4f));
        DrawRectangle((int)card.x, (int)card.y, 6, (int)card.height, Fade(wi.color, unlocked ? 1.0f : 0.4f));

        // Ícone + tecla
        DrawWeaponGlyph(s + 1, (Vector2){ card.x + 50, card.y + 56 }, unlocked ? wi.color : Fade(GRAY, 0.7f));
        DrawRectangleRounded((Rectangle){ card.x + 28, card.y + 96, 44, 30 }, 0.3f, 6, Fade(wi.color, 0.2f));
        DrawTextEx(font, TextFormat("[%d]", wi.key), (Vector2){ card.x + 36, card.y + 100 }, 22.0f, 1.0f, wi.color);

        // Nome
        DrawTextEx(font, wi.name, (Vector2){ card.x + 96, card.y + 16 }, 24.0f, 1.0f, unlocked ? WHITE : Fade(GRAY, 0.8f));
        DrawTextEx(font, wi.desc, (Vector2){ card.x + 96, card.y + 46 }, 15.0f, 1.0f, Fade(WHITE, 0.75f));

        // Stats
        float sx = card.x + 96, sy = card.y + 78;
        DrawTextEx(font, TextFormat("Dano base: %d  (+ ATK do nivel)", wi.baseDamage), (Vector2){ sx, sy }, 15.0f, 1.0f, GOLD);
        DrawTextEx(font, TextFormat("Cadencia: %s", wi.speedTxt), (Vector2){ sx, sy + 22 }, 15.0f, 1.0f, THEME_COLOR_MAIN);
        DrawTextEx(font, TextFormat("Cooldown: %.2fs", wi.cooldown), (Vector2){ sx + 230, sy + 22 }, 15.0f, 1.0f, THEME_COLOR_MAIN);
        DrawTextEx(font, TextFormat("Especial: %s", wi.special), (Vector2){ sx, sy + 44 }, 15.0f, 1.0f, (Color){ 120, 230, 140, 255 });
        DrawTextEx(font, TextFormat("Estilo: %s", wi.playstyle), (Vector2){ sx, sy + 66 }, 14.0f, 1.0f, Fade(WHITE, 0.7f));

        // Rodapé do card: desbloqueio
        Rectangle foot = { card.x + 6, card.y + cardH - 30, cardW - 12, 24 };
        if (unlocked)
        {
            DrawTextEx(font, (wi.unlockLevel <= 1) ? "DISPONIVEL DESDE O INICIO" : TextFormat("DESBLOQUEADA (Nivel %d)", wi.unlockLevel),
                       (Vector2){ foot.x + 10, foot.y + 4 }, 15.0f, 1.0f, (Color){ 80, 220, 120, 255 });
        }
        else
        {
            DrawRectangleRounded(foot, 0.4f, 6, Fade(RED, 0.12f));
            DrawTextEx(font, TextFormat("BLOQUEADA - desbloqueia no Nivel %d", wi.unlockLevel),
                       (Vector2){ foot.x + 10, foot.y + 4 }, 15.0f, 1.0f, (Color){ 230, 120, 120, 255 });
        }
    }

    DrawButton(arsenalBack, font, true);
    DrawTextEx(font, "ESC para voltar", (Vector2){ 20, SCREEN_HEIGHT - 30 }, 14.0f, 1.0f, DARKGRAY);
}

void UpdateTelaArsenal(GameState *game, Vector2 mouse)
{
    arsenalBack.hover = CheckCollisionPointRec(mouse, arsenalBack.bounds);
    if ((arsenalBack.hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ESCAPE))
    {
        game->currentScreen = SCREEN_MENU;
    }
}
