// tela_skins.c
// Tela dedicada de seleção de SKINS do jogador (com preview ao vivo do modelo)
// e da skin da ARMA (cor dos projéteis/lâmina). Mostra claramente qual está ativa.
#include "../../include/telas.h"
#include "../../include/gameplay.h"
#include "../../Assets/@models/player_model.h"
#include <stdio.h>

static UIButton skinsBack = { { 490, 650, 300, 46 }, "VOLTAR E SALVAR", false, false };

static const char *PlayerSkinDesc(int id)
{
    switch (id)
    {
        case 1:  return "Jaleco branco, cruz vermelha. Estilo equipe medica.";
        case 2:  return "Armadura roxa corrompida com brilho toxico.";
        default: return "Cavaleiro imune classico, equilibrado.";
    }
}

static const char *WeaponSkinDesc(int id)
{
    switch (id)
    {
        case 1:  return "Plasma: projeteis em magenta/ciano.";
        case 2:  return "Toxica: tons verde-acido.";
        default: return "Padrao: azul-imune.";
    }
}

void DrawTelaSkins(GameState *game, Font font)
{
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, THEME_COLOR_BG_DARK, THEME_COLOR_BG_LIGHT);

    const char *title = "SELECAO DE SKINS";
    Vector2 tSz = MeasureTextEx(font, title, 40.0f, 1.5f);
    DrawTextEx(font, title, (Vector2){ SCREEN_WIDTH / 2.0f - tSz.x / 2.0f, 36.0f }, 40.0f, 1.5f, THEME_COLOR_TEXT);
    const char *sub = "Todas as skins estao liberadas. Sua escolha vale na proxima partida.";
    Vector2 sSz = MeasureTextEx(font, sub, 16.0f, 1.0f);
    DrawTextEx(font, sub, (Vector2){ SCREEN_WIDTH / 2.0f - sSz.x / 2.0f, 86.0f }, 16.0f, 1.0f, Fade(WHITE, 0.8f));

    // ---- PREVIEW grande do herói (à esquerda) ----
    Rectangle preview = { 110, 150, 380, 380 };
    DrawRectangleRounded(preview, 0.05f, 8, Fade((Color){ 8, 16, 12, 255 }, 0.8f));
    DrawRectangleRoundedLines(preview, 0.05f, 8, THEME_COLOR_MAIN);
    DrawTextEx(font, "PREVIEW", (Vector2){ preview.x + 16, preview.y + 12 }, 16.0f, 1.0f, Fade(THEME_COLOR_MAIN, 0.8f));

    // BUGFIX: no menu o GameState está zerado, então squashX/squashY = 0 e o
    // modelo era escalado para 0 (invisível). Forçamos valores neutros para que
    // o preview apareça e reflita a skin/arma selecionada em tempo real.
    Player tmp = game->player;
    tmp.position = (Vector2){ preview.x + preview.width / 2.0f, preview.y + preview.height / 2.0f + 20.0f };
    tmp.isMoving = false;
    tmp.facingDir = 1;
    tmp.squashX = 1.0f;
    tmp.squashY = 1.0f;
    tmp.attackBoostTimer = 0.0f;
    DrawPlayerModel(&tmp, 95.0f, THEME_COLOR_MAIN, (float)GetTime(), 0.0f);

    DrawTextEx(font, PlayerSkinName(game->player.skinId),
               (Vector2){ preview.x + 16, preview.y + preview.height - 60 }, 22.0f, 1.0f, GOLD);
    // Indica a skin de arma ativa (a lâmina do preview já usa a cor selecionada)
    DrawTextEx(font, TextFormat("Arma: %s", WeaponSkinName(game->player.weaponSkinId)),
               (Vector2){ preview.x + 16, preview.y + preview.height - 32 }, 16.0f, 1.0f,
               WeaponSkinPrimary(game->player.weaponSkinId));

    // ---- Cards de SKIN do jogador (à direita) ----
    DrawTextEx(font, "SKIN DO ANTICORPO", (Vector2){ 560, 150 }, 20.0f, 1.0f, THEME_COLOR_MAIN);
    for (int i = 0; i < SKIN_COUNT; i++)
    {
        Rectangle card = { 560, 184.0f + i * 86.0f, 600, 74 };
        bool active = (game->player.skinId == i);
        DrawRectangleRounded(card, 0.15f, 6, active ? Fade(THEME_COLOR_MAIN, 0.18f) : Fade((Color){ 12, 12, 22, 255 }, 0.8f));
        DrawRectangleRoundedLines(card, 0.15f, 6, active ? THEME_COLOR_MAIN : THEME_COLOR_BORDER);

        DrawTextEx(font, PlayerSkinName(i), (Vector2){ card.x + 18, card.y + 12 }, 22.0f, 1.0f, active ? THEME_COLOR_MAIN : WHITE);
        DrawTextEx(font, PlayerSkinDesc(i), (Vector2){ card.x + 18, card.y + 42 }, 14.0f, 1.0f, Fade(WHITE, 0.7f));
        if (active)
            DrawTextEx(font, "ATIVA", (Vector2){ card.x + card.width - 80, card.y + 26 }, 18.0f, 1.0f, GOLD);
        else
            DrawTextEx(font, "Clique", (Vector2){ card.x + card.width - 84, card.y + 28 }, 14.0f, 1.0f, Fade(WHITE, 0.5f));
    }

    // ---- Skin da ARMA (linha de swatches) ----
    DrawTextEx(font, "SKIN DA ARMA (cor dos disparos):", (Vector2){ 560, 470 }, 18.0f, 1.0f, THEME_COLOR_MAIN);
    for (int i = 0; i < WEAPON_SKIN_COUNT; i++)
    {
        Rectangle sw = { 560.0f + i * 200.0f, 502, 188, 56 };
        bool active = (game->player.weaponSkinId == i);
        DrawRectangleRounded(sw, 0.2f, 6, active ? Fade(WeaponSkinPrimary(i), 0.25f) : Fade((Color){ 12, 12, 22, 255 }, 0.8f));
        DrawRectangleRoundedLines(sw, 0.2f, 6, active ? WeaponSkinPrimary(i) : THEME_COLOR_BORDER);
        DrawCircleV((Vector2){ sw.x + 26, sw.y + 28 }, 12.0f, WeaponSkinPrimary(i));
        DrawCircleLines((int)(sw.x + 26), (int)(sw.y + 28), 12.0f, WeaponSkinSecondary(i));
        DrawTextEx(font, WeaponSkinName(i), (Vector2){ sw.x + 48, sw.y + 8 }, 16.0f, 1.0f, active ? WeaponSkinPrimary(i) : WHITE);
        DrawTextEx(font, WeaponSkinDesc(i), (Vector2){ sw.x + 48, sw.y + 30 }, 11.0f, 1.0f, Fade(WHITE, 0.65f));
    }

    DrawButton(skinsBack, font, true);
    DrawTextEx(font, "ESC para voltar", (Vector2){ 20, SCREEN_HEIGHT - 30 }, 14.0f, 1.0f, DARKGRAY);
}

void UpdateTelaSkins(GameState *game, Vector2 mouse)
{
    // Seleção de skin do jogador
    for (int i = 0; i < SKIN_COUNT; i++)
    {
        Rectangle card = { 560, 184.0f + i * 86.0f, 600, 74 };
        if (CheckCollisionPointRec(mouse, card) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            game->player.skinId = i;
            SavePlayerConfig(game);
        }
    }

    // Seleção de skin da arma
    for (int i = 0; i < WEAPON_SKIN_COUNT; i++)
    {
        Rectangle sw = { 560.0f + i * 200.0f, 502, 188, 56 };
        if (CheckCollisionPointRec(mouse, sw) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            game->player.weaponSkinId = i;
            SavePlayerConfig(game);
        }
    }

    skinsBack.hover = CheckCollisionPointRec(mouse, skinsBack.bounds);
    if ((skinsBack.hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ESCAPE))
    {
        SavePlayerConfig(game);
        game->currentScreen = SCREEN_MENU;
    }
}
