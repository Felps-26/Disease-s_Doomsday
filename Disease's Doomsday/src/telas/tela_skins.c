// tela_skins.c
// GUARDA-ROUPA modular do Anticorpo: personalização por partes (capacete, facial,
// peitoral, braços, calças, botas, efeito) + variações de material (paleta do
// corpo) e cor da arma. Preview ao vivo, itens bloqueados com explicação,
// equipar/remover/restaurar, persistência, mouse + teclado.
//
// É DATA-DRIVEN: as peças vêm do catálogo (Assets/@models/cosmetics.c) e o estado
// equipado vive em player->cosmetics[]. O layout usa CONSTANTES centralizadas
// (sem coordenadas frágeis espalhadas). A categoria "Traseiro" (COS_BACK) foi
// removida do jogo: não aparece aqui e é ignorada ao carregar (compat. de saves).
#include "../../include/telas.h"
#include "../../include/gameplay.h"
#include "../../include/asset_manager.h"
#include "../../Assets/@models/player_model.h"
#include "../../Assets/@models/cosmetics.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

extern Vector2 g_virtualMouse; // mouse já corrigido pelo letterbox (main.c)

// ---- Slots VISÍVEIS na UI (ordem de exibição; exclui COS_BACK) ----
static const CosmeticSlot WB_VISIBLE[] = {
    COS_HELMET, COS_FACE, COS_CHEST, COS_ARMS, COS_LEGS, COS_BOOTS, COS_FX
};
#define WB_VIS_COUNT ((int)(sizeof(WB_VISIBLE) / sizeof(WB_VISIBLE[0])))

// ---- Layout CENTRALIZADO (coordenadas virtuais 1280x720, letterboxed) ----
#define WB_LX          44.0f    // coluna esquerda: x e largura
#define WB_LW         348.0f
#define WB_MAT_Y      156.0f    // seletores Padrão/Médica/Infectada (ACIMA do preview)
#define WB_MAT_H       44.0f
#define WB_PV_Y       212.0f    // quadrado de preview
#define WB_PV_H       300.0f
#define WB_WPN_LBL_Y  524.0f    // arma (ABAIXO do preview)
#define WB_WPN_Y      546.0f
#define WB_WPN_H       40.0f
#define WB_TAB_X      410.0f    // coluna direita: abas de categoria
#define WB_TAB_W      150.0f
#define WB_TAB_Y      156.0f
#define WB_TAB_H       44.0f
#define WB_TAB_PITCH   50.0f
#define WB_LIST_X     576.0f    // grade de itens
#define WB_LIST_W     560.0f
#define WB_LIST_Y     184.0f
#define WB_CARD_H      70.0f
#define WB_CARD_PITCH  78.0f

// ---- Estado de UI (transitório) ----
static int   wbSel    = 0;     // índice em WB_VISIBLE (categoria selecionada)
static int   wbFocus  = 0;     // item em foco dentro do slot
static float wbDenied = 0.0f;  // feedback "bloqueado"
static float wbToast  = 0.0f;  // timer do toast
static char  wbToastMsg[56] = { 0 };

static CosmeticSlot CurSlot(void) { return WB_VISIBLE[(wbSel < 0 || wbSel >= WB_VIS_COUNT) ? 0 : wbSel]; }
static int EffLevel(GameState *game) { return (game->player.level > 1) ? game->player.level : 1; }
static void Toast(const char *m) { snprintf(wbToastMsg, sizeof(wbToastMsg), "%s", m); wbToast = 1.6f; }

// ---- Geometria ÚNICA (desenho == hitbox) ----
static Rectangle PreviewRect(void)       { return (Rectangle){ WB_LX, WB_PV_Y, WB_LW, WB_PV_H }; }
static Rectangle MaterialSwatch(int i)   { return (Rectangle){ WB_LX + i * 119.0f, WB_MAT_Y, 110.0f, WB_MAT_H }; }
static Rectangle WeaponSwatchRect(int i) { return (Rectangle){ WB_LX + i * 119.0f, WB_WPN_Y, 110.0f, WB_WPN_H }; }
static Rectangle SlotTabRect(int i)      { return (Rectangle){ WB_TAB_X, WB_TAB_Y + i * WB_TAB_PITCH, WB_TAB_W, WB_TAB_H }; }
static Rectangle ItemCardRect(int i)     { return (Rectangle){ WB_LIST_X, WB_LIST_Y + i * WB_CARD_PITCH, WB_LIST_W, WB_CARD_H }; }
static Rectangle BtnEquipRect(void)      { return (Rectangle){ WB_LIST_X, 600, 168, 44 }; }
static Rectangle BtnRemoveRect(void)     { return (Rectangle){ WB_LIST_X + 178, 600, 168, 44 }; }
static Rectangle BtnRestoreRect(void)    { return (Rectangle){ WB_LIST_X + 356, 600, 204, 44 }; }
static Rectangle BtnBackRect(void)       { return (Rectangle){ WB_TAB_X, 656, WB_LIST_X + WB_LIST_W - WB_TAB_X, 44 }; }

void DrawTelaSkins(GameState *game, Font font)
{
    float time = (float)GetTime();
    DrawThemedBackground(SCREEN_SKINS, time, game->screenAnim / 0.4f);
    float entry = UIEase(game->screenAnim / 0.4f);

    // Título + subtítulo em DUAS linhas, abaixo do sublinhado do título (sem
    // sobreposição com a coluna direita).
    DrawUIScreenTitle(font, "GUARDA-ROUPA DO ANTICORPO", (Color){ 200, 110, 255, 255 }, entry);
    const char *sub1 = "Personalize o Anticorpo por partes — as pecas se combinam livremente.";
    const char *sub2 = "Sua escolha vale na proxima partida.";
    Vector2 s1 = MeasureTextEx(font, sub1, 15.0f, 1.0f);
    Vector2 s2 = MeasureTextEx(font, sub2, 15.0f, 1.0f);
    DrawTextEx(font, sub1, (Vector2){ SCREEN_WIDTH / 2.0f - s1.x / 2.0f, 100.0f }, 15.0f, 1.0f, Fade(WHITE, 0.85f));
    DrawTextEx(font, sub2, (Vector2){ SCREEN_WIDTH / 2.0f - s2.x / 2.0f, 122.0f }, 15.0f, 1.0f, Fade(WHITE, 0.6f));

    int lvl = EffLevel(game);

    // ============================ COLUNA ESQUERDA ============================
    // 1) Seletores de MATERIAL (Padrão/Médica/Infectada) ACIMA do preview.
    for (int i = 0; i < SKIN_COUNT; i++)
    {
        Rectangle sw = MaterialSwatch(i);
        bool active = (game->player.skinId == i);
        bool hov = CheckCollisionPointRec(g_virtualMouse, sw);
        DrawRectangleRounded(sw, 0.2f, 6, active ? Fade(THEME_COLOR_MAIN, 0.22f)
                                               : hov ? Fade((Color){ 40, 60, 80, 255 }, 0.7f)
                                                     : Fade((Color){ 12, 12, 22, 255 }, 0.85f));
        DrawRectangleRoundedLines(sw, 0.2f, 6, active ? THEME_COLOR_MAIN : THEME_COLOR_BORDER);
        if (active) DrawRectangleRoundedLines(sw, 0.2f, 6, Fade(WHITE, 0.4f));
        const char *nm = PlayerSkinName(i);
        Vector2 nsz = MeasureTextEx(font, nm, 16.0f, 1.0f);
        DrawTextEx(font, nm, (Vector2){ sw.x + sw.width / 2 - nsz.x / 2, sw.y + 6 }, 16.0f, 1.0f, active ? THEME_COLOR_MAIN : WHITE);
        const char *tag = active ? "ATIVO" : "clique";
        Vector2 tsz = MeasureTextEx(font, tag, 12.0f, 1.0f);
        DrawTextEx(font, tag, (Vector2){ sw.x + sw.width / 2 - tsz.x / 2, sw.y + 26 }, 12.0f, 1.0f, active ? GOLD : Fade(WHITE, 0.5f));
    }

    // 2) Quadrado de PREVIEW com o personagem centralizado (sem rótulo "Preview").
    Rectangle pv = PreviewRect();
    DrawRectangleRounded(pv, 0.06f, 8, Fade((Color){ 8, 16, 22, 255 }, 0.85f));
    DrawRectangleRoundedLines(pv, 0.06f, 8, THEME_COLOR_MAIN);
    DrawEllipse((int)(pv.x + pv.width / 2.0f), (int)(pv.y + pv.height - 54.0f), 96.0f, 22.0f, Fade(THEME_COLOR_MAIN, 0.10f));

    Player tmp = game->player; // cópia: reflete skin + cosméticos equipados
    tmp.position = (Vector2){ pv.x + pv.width / 2.0f, pv.y + pv.height / 2.0f + 2.0f };
    tmp.isMoving = false; tmp.facingDir = 1;
    tmp.squashX = 1.0f; tmp.squashY = 1.0f; tmp.attackBoostTimer = 0.0f;
    DrawPlayerModel(&tmp, 92.0f, THEME_COLOR_MAIN, time, 0.0f);

    // Nome da skin atual (canto inferior do quadro, sem invadir).
    DrawTextEx(font, PlayerSkinName(game->player.skinId),
               (Vector2){ pv.x + 14, pv.y + pv.height - 26 }, 18.0f, 1.0f, GOLD);

    // 3) Escolha da ARMA (cor dos disparos) ABAIXO do preview, com margem clara.
    DrawTextEx(font, "ARMA (cor dos disparos)", (Vector2){ WB_LX, WB_WPN_LBL_Y }, 15.0f, 1.0f, THEME_COLOR_MAIN);
    for (int i = 0; i < WEAPON_SKIN_COUNT; i++)
    {
        Rectangle sw = WeaponSwatchRect(i);
        bool active = (game->player.weaponSkinId == i);
        bool hov = CheckCollisionPointRec(g_virtualMouse, sw);
        DrawRectangleRounded(sw, 0.25f, 6, active ? Fade(WeaponSkinPrimary(i), 0.3f)
                                                : hov ? Fade((Color){ 40, 60, 80, 255 }, 0.7f)
                                                      : Fade((Color){ 12, 12, 22, 255 }, 0.85f));
        DrawRectangleRoundedLines(sw, 0.25f, 6, active ? WeaponSkinPrimary(i) : THEME_COLOR_BORDER);
        DrawCircleV((Vector2){ sw.x + 18, sw.y + sw.height / 2 }, 9.0f, WeaponSkinPrimary(i));
        DrawCircleLines((int)(sw.x + 18), (int)(sw.y + sw.height / 2), 9.0f, WeaponSkinSecondary(i));
        DrawTextEx(font, WeaponSkinName(i), (Vector2){ sw.x + 34, sw.y + 11 }, 13.0f, 1.0f, active ? WeaponSkinPrimary(i) : WHITE);
    }

    // ============================ COLUNA DIREITA ============================
    // Abas de categoria (slots visíveis).
    for (int i = 0; i < WB_VIS_COUNT; i++)
    {
        Rectangle tab = SlotTabRect(i);
        CosmeticSlot slot = WB_VISIBLE[i];
        bool sel = (i == wbSel);
        bool hov = CheckCollisionPointRec(g_virtualMouse, tab);
        DrawRectangleRounded(tab, 0.25f, 5, sel ? Fade((Color){ 190, 110, 255, 255 }, 0.25f)
                                               : hov ? Fade((Color){ 120, 80, 160, 255 }, 0.18f)
                                                     : Fade((Color){ 12, 12, 22, 255 }, 0.8f));
        DrawRectangleRoundedLines(tab, 0.25f, 5, sel ? (Color){ 200, 130, 255, 255 } : THEME_COLOR_BORDER);
        if (sel) DrawRectangle((int)tab.x, (int)tab.y, 4, (int)tab.height, (Color){ 200, 130, 255, 255 });
        DrawTextEx(font, CosmeticSlotShort(slot), (Vector2){ tab.x + 14, tab.y + 12 }, 16.0f, 1.0f, sel ? WHITE : Fade(WHITE, 0.8f));
        if (game->player.cosmetics[slot] > 0)
            DrawCircleV((Vector2){ tab.x + tab.width - 14, tab.y + tab.height / 2 }, 4.0f, (Color){ 0, 220, 120, 255 });
    }

    // Cabeçalho do slot + itens.
    CosmeticSlot cur = CurSlot();
    DrawTextEx(font, CosmeticSlotName(cur), (Vector2){ WB_LIST_X, 150 }, 18.0f, 1.0f, (Color){ 200, 130, 255, 255 });
    int count = CosmeticItemCount(cur);
    int equipped = game->player.cosmetics[cur];
    for (int i = 0; i < count; i++)
    {
        Rectangle card = ItemCardRect(i);
        const CosmeticItem *it = CosmeticGet(cur, i);
        if (!it) continue;
        bool unlocked = CosmeticUnlocked(cur, i, lvl);
        bool isEquip = (i == equipped);
        bool isFocus = (i == wbFocus);

        Color border = isEquip ? (Color){ 0, 220, 120, 255 } : isFocus ? (Color){ 200, 130, 255, 255 } : THEME_COLOR_BORDER;
        Color bg = isEquip ? Fade((Color){ 0, 220, 120, 255 }, 0.12f)
                 : isFocus ? Fade((Color){ 190, 110, 255, 255 }, 0.12f)
                           : Fade((Color){ 12, 12, 22, 255 }, 0.82f);
        DrawRectangleRounded(card, 0.12f, 6, bg);
        DrawRectangleRoundedLines(card, 0.12f, 6, border);
        if (isFocus) DrawRectangleRoundedLines(card, 0.12f, 6, Fade(WHITE, 0.3f + 0.3f * sinf(time * 6.0f)));

        DrawRectangleRounded((Rectangle){ card.x + 12, card.y + 16, 38, 38 }, 0.3f, 5, Fade(it->tint, unlocked ? 0.9f : 0.3f));
        DrawRectangleRoundedLines((Rectangle){ card.x + 12, card.y + 16, 38, 38 }, 0.3f, 5, Fade(WHITE, 0.4f));

        DrawTextEx(font, it->name, (Vector2){ card.x + 62, card.y + 12 }, 18.0f, 1.0f, unlocked ? WHITE : Fade(WHITE, 0.5f));
        DrawTextWrapped(font, it->desc, (Rectangle){ card.x + 62, card.y + 36, card.width - 190, 30 }, 13.0f, 1.0f, Fade(WHITE, unlocked ? 0.72f : 0.4f));

        if (!unlocked)
        {
            DrawTextEx(font, TextFormat("Nivel %d", it->unlockLevel), (Vector2){ card.x + card.width - 100, card.y + 14 }, 14.0f, 1.0f, (Color){ 255, 120, 120, 255 });
            DrawRectangleRounded((Rectangle){ card.x + card.width - 100, card.y + 40, 16, 14 }, 0.3f, 4, Fade(GRAY, 0.9f));
            DrawRectangleLines((int)(card.x + card.width - 98), (int)(card.y + 34), 12, 9, Fade(GRAY, 0.9f));
        }
        else if (isEquip)
            DrawTextEx(font, "EQUIPADO", (Vector2){ card.x + card.width - 102, card.y + 14 }, 14.0f, 1.0f, (Color){ 0, 230, 130, 255 });
        else
            DrawTextEx(font, "clique", (Vector2){ card.x + card.width - 70, card.y + 16 }, 13.0f, 1.0f, Fade(WHITE, 0.5f));
    }

    // Dica de teclado (abaixo da grade, sem sobrepor botões).
    DrawTextEx(font, "Setas: navegar   |   ENTER: equipar   |   R: remover   |   ESC: voltar",
               (Vector2){ WB_LIST_X, WB_LIST_Y + 4 * WB_CARD_PITCH + 6 }, 13.0f, 1.0f, Fade(WHITE, 0.55f));

    // Botões de ação.
    Vector2 mp = g_virtualMouse;
    UIButton bEquip   = { BtnEquipRect(),   "EQUIPAR",  CheckCollisionPointRec(mp, BtnEquipRect()),   false };
    UIButton bRemove  = { BtnRemoveRect(),  "REMOVER",  CheckCollisionPointRec(mp, BtnRemoveRect()),  false };
    UIButton bRestore = { BtnRestoreRect(), "RESTAURAR PADRAO", CheckCollisionPointRec(mp, BtnRestoreRect()), false };
    UIButton bBack    = { BtnBackRect(),    "VOLTAR E SALVAR",  CheckCollisionPointRec(mp, BtnBackRect()), false };
    DrawButton(bEquip, font, true);
    DrawButton(bRemove, font, true);
    DrawButton(bRestore, font, true);
    DrawButton(bBack, font, true);

    // Toast / feedback.
    if (wbToast > 0.0f)
    {
        float a = wbToast > 1.4f ? (1.6f - wbToast) / 0.2f : (wbToast < 0.3f ? wbToast / 0.3f : 1.0f);
        Vector2 tz = MeasureTextEx(font, wbToastMsg, 18.0f, 1.0f);
        Rectangle tb = { SCREEN_WIDTH / 2.0f - tz.x / 2.0f - 16, 686, tz.x + 32, 30 };
        DrawRectangleRounded(tb, 0.5f, 6, Fade((Color){ 10, 10, 20, 255 }, 0.9f * a));
        DrawRectangleRoundedLines(tb, 0.5f, 6, Fade((Color){ 255, 120, 120, 255 }, a));
        DrawTextEx(font, wbToastMsg, (Vector2){ SCREEN_WIDTH / 2.0f - tz.x / 2.0f, 691 }, 18.0f, 1.0f, Fade(WHITE, a));
    }
}

// Equipa o item em foco (se liberado); feedback visual + sonoro.
static void EquipFocused(GameState *game)
{
    CosmeticSlot cur = CurSlot();
    if (!CosmeticUnlocked(cur, wbFocus, EffLevel(game)))
    {
        wbDenied = 0.5f;
        Toast("Item bloqueado: suba de nivel para liberar");
        return;
    }
    if (game->player.cosmetics[cur] != wbFocus)
    {
        game->player.cosmetics[cur] = wbFocus;
        SavePlayerConfig(game);
        if (g_assets.sfxPickup.frameCount > 0) PlaySound(g_assets.sfxPickup);
    }
}

static void RemoveCurrent(GameState *game)
{
    game->player.cosmetics[CurSlot()] = 0;
    SavePlayerConfig(game);
    if (g_assets.sfxPickup.frameCount > 0) PlaySound(g_assets.sfxPickup);
    Toast("Peca removida");
}

void UpdateTelaSkins(GameState *game, Vector2 mouse)
{
    float dt = GetFrameTime();
    if (wbToast > 0.0f) wbToast -= dt;
    if (wbDenied > 0.0f) wbDenied -= dt;
    if (wbSel < 0 || wbSel >= WB_VIS_COUNT) wbSel = 0;

    CosmeticSlot cur = CurSlot();
    int count = CosmeticItemCount(cur);
    if (count < 1) count = 1;
    if (wbFocus >= count) wbFocus = count - 1;
    if (wbFocus < 0) wbFocus = 0;

    // ---- Teclado: navegação + ações ----
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
    { wbSel = (wbSel + 1) % WB_VIS_COUNT; wbFocus = game->player.cosmetics[CurSlot()]; }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
    { wbSel = (wbSel + WB_VIS_COUNT - 1) % WB_VIS_COUNT; wbFocus = game->player.cosmetics[CurSlot()]; }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) wbFocus = (wbFocus + 1) % count;
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))  wbFocus = (wbFocus + count - 1) % count;
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) EquipFocused(game);
    if (IsKeyPressed(KEY_R)) RemoveCurrent(game);

    // ---- Mouse: abas de categoria ----
    for (int i = 0; i < WB_VIS_COUNT; i++)
        if (CheckCollisionPointRec(mouse, SlotTabRect(i)) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        { wbSel = i; wbFocus = game->player.cosmetics[WB_VISIBLE[i]]; }

    // ---- Mouse: cards de item (hover foca; clique equipa) ----
    int cnt = CosmeticItemCount(CurSlot());
    for (int i = 0; i < cnt; i++)
        if (CheckCollisionPointRec(mouse, ItemCardRect(i)))
        {
            wbFocus = i;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) EquipFocused(game);
        }

    // ---- Material (paleta) + arma ----
    for (int i = 0; i < SKIN_COUNT; i++)
        if (CheckCollisionPointRec(mouse, MaterialSwatch(i)) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        { game->player.skinId = i; SavePlayerConfig(game); if (g_assets.sfxPickup.frameCount > 0) PlaySound(g_assets.sfxPickup); }
    for (int i = 0; i < WEAPON_SKIN_COUNT; i++)
        if (CheckCollisionPointRec(mouse, WeaponSwatchRect(i)) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        { game->player.weaponSkinId = i; SavePlayerConfig(game); if (g_assets.sfxPickup.frameCount > 0) PlaySound(g_assets.sfxPickup); }

    // ---- Botões ----
    if (CheckCollisionPointRec(mouse, BtnEquipRect())   && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) EquipFocused(game);
    if (CheckCollisionPointRec(mouse, BtnRemoveRect())  && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) RemoveCurrent(game);
    if (CheckCollisionPointRec(mouse, BtnRestoreRect()) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        for (int s = 0; s < COS_SLOT_COUNT; s++) game->player.cosmetics[s] = 0;
        game->player.skinId = 0; game->player.weaponSkinId = 0;
        wbFocus = 0;
        SavePlayerConfig(game);
        if (g_assets.sfxPickup.frameCount > 0) PlaySound(g_assets.sfxPickup);
        Toast("Aparencia restaurada ao padrao");
    }

    bool backClick = CheckCollisionPointRec(mouse, BtnBackRect()) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    if (backClick || IsKeyPressed(KEY_ESCAPE))
    {
        SavePlayerConfig(game);
        game->currentScreen = SCREEN_MENU;
    }
}
