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

// DrawSciFiBox definition will be moved to telas.c but made non-static, so we declare it here or in telas.h
extern void DrawSciFiBox(Rectangle r, Color col);

void DrawStatusIcons(Vector2 pos, bool poisoned, bool slowed, float time)
{
    float offsetY = 40.0f + sinf(time * 5.0f) * 5.0f;
    Vector2 iconPos = { pos.x, pos.y - offsetY };
    
    int count = 0;
    if (poisoned) count++;
    if (slowed) count++;
    
    if (count == 0) return;
    
    float startX = iconPos.x - ((count - 1) * 12.0f);
    
    int current = 0;
    if (poisoned) {
        Vector2 p = { startX + current * 24.0f, iconPos.y };
        DrawCircleV(p, 8.0f, Fade(PURPLE, 0.8f));
        DrawCircleV(p, 6.0f, Fade(MAGENTA, 0.9f));
        DrawRectangle(p.x - 2, p.y - 4, 4, 8, WHITE);
        DrawRectangle(p.x - 2, p.y + 6, 4, 2, WHITE); // gota shape approximation
        current++;
    }
    if (slowed) {
        Vector2 p = { startX + current * 24.0f, iconPos.y };
        DrawCircleV(p, 8.0f, Fade(BLUE, 0.8f));
        DrawCircleV(p, 6.0f, Fade(SKYBLUE, 0.9f));
        DrawLineEx((Vector2){p.x - 4, p.y}, (Vector2){p.x + 4, p.y}, 2.0f, WHITE);
        current++;
    }
}

// ============================================================================
// AUXILIAR: DESENHAR O JOGADOR DE ACORDO COM A SKIN ATUAL
// ============================================================================
void DrawPlayerHero(GameState *game, Vector2 pPos, float playerSize)
{
    // Rastro (Trail Render)
    rlBegin(RL_QUADS);
    for (int i = 0; i < 9; i++) {
        int idx1 = (game->player.trailIndex + i) % 10;
        int idx2 = (game->player.trailIndex + i + 1) % 10;
        Vector2 p1 = game->player.trail[idx1];
        Vector2 p2 = game->player.trail[idx2];
        
        float t1 = (float)i / 9.0f;
        float t2 = (float)(i + 1) / 9.0f;
        
        float thick1 = 2.0f + 18.0f * t1;
        float thick2 = 2.0f + 18.0f * t2;
        
        Color c1 = Fade((Color){ 0, 229, 255, 255 }, t1 * 0.5f);
        Color c2 = Fade((Color){ 0, 229, 255, 255 }, t2 * 0.5f);
        
        Vector2 dir = Vector2Subtract(p2, p1);
        if (Vector2LengthSqr(dir) > 0.01f) {
            dir = Vector2Normalize(dir);
            Vector2 normal = { -dir.y, dir.x };
            
            rlColor4ub(c1.r, c1.g, c1.b, c1.a);
            rlVertex2f(p1.x + normal.x * thick1, p1.y + normal.y * thick1);
            rlVertex2f(p1.x - normal.x * thick1, p1.y - normal.y * thick1);
            
            rlColor4ub(c2.r, c2.g, c2.b, c2.a);
            rlVertex2f(p2.x - normal.x * thick2, p2.y - normal.y * thick2);
            rlVertex2f(p2.x + normal.x * thick2, p2.y + normal.y * thick2);
        }
    }
    rlEnd();

    bool isBoosted = (game->player.attackBoostTimer > 0.0f);
    Color pCol = isBoosted ? GOLD : THEME_COLOR_MAIN;
    
    // Renderiza a forma base do Herói (Cavaleiro Branco)
    // Passa o tamanho maior (60.0f em vez de playerSize) para mais detalhes
    DrawPlayerModel(&game->player, 60.0f, pCol, GetTime(), game->slashAnimTimer);
}

void DrawHUD(GameState *game, Font font)
{
    // A. BARRA DE STATUS DO JOGADOR (HP & XP)
    DrawSciFiBox((Rectangle){ 20, 20, 360, 95 }, THEME_COLOR_MAIN);

    // Informações básicas (Nome à esquerda, Nível à direita)
    DrawTextEx(font, game->player.name, (Vector2){ 35, 28 }, 18.0f, 1.0f, THEME_COLOR_MAIN);
    
    char lvlStr[16];
    sprintf(lvlStr, "LV. %d", game->player.level);
    Vector2 lvlSize = MeasureTextEx(font, lvlStr, 14.0f, 1.0f);
    DrawTextEx(font, lvlStr, (Vector2){ 365.0f - lvlSize.x, 30.0f }, 14.0f, 1.0f, GOLD);

    // Barra de HP
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

    // Barra de XP
    DrawTextEx(font, "XP", (Vector2){ 35, 73 }, 11.0f, 1.0f, (Color){ 224, 64, 251, 255 });
    DrawRectangleRounded((Rectangle){ 60, 75, 305, 6 }, 0.5f, 4, BLACK);
    float xpPercent = (float)game->player.xp / game->player.xpNeeded;
    if (xpPercent > 0.0f)
    {
        DrawRectangleRounded((Rectangle){ 60, 75, 305.0f * xpPercent, 6 }, 0.5f, 4, (Color){ 224, 64, 251, 255 });
    }

    // B. PAINEL DE ONDA / HORDA
    Rectangle waveBox = { 490, 20, 300, 60 };
    DrawSciFiBox(waveBox, THEME_COLOR_MAIN);

    const char *waveTxt = TextFormat("ONDA DE INFECÇÃO: %d / 5", game->wave);
    Vector2 waveTxtSize = MeasureTextEx(font, waveTxt, 16.0f, 1.0f);
    DrawTextEx(font, waveTxt, (Vector2){ 640.0f - waveTxtSize.x / 2.0f, 28.0f }, 16.0f, 1.0f, GOLD);

    const char *remTxt = TextFormat("Patógenos Ativos: %d", game->enemiesRemaining);
    Vector2 remTxtSize = MeasureTextEx(font, remTxt, 13.0f, 1.0f);
    DrawTextEx(font, remTxt, (Vector2){ 640.0f - remTxtSize.x / 2.0f, 48.0f }, 13.0f, 1.0f, WHITE);

    // C. PONTUAÇÃO (SUPERIOR DIREITO)
    DrawSciFiBox((Rectangle){ 900, 20, 150, 55 }, THEME_COLOR_MAIN);
    DrawTextEx(font, "SCORE", (Vector2){ 915, 27 }, 12.0f, 1.0f, GRAY);
    DrawTextEx(font, TextFormat("%06d", game->player.score), (Vector2){ 915, 42 }, 20.0f, 1.0f, YELLOW);

    // C.2. BARRA DE VIDA DO CHEFE (fixa no topo, visível mesmo com o chefe fora da tela)
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (game->enemies[i].active && game->enemies[i].tier == TIER_3_BOSS && game->enemies[i].state != DEATH)
        {
            float bossPct = (float)game->enemies[i].hp / game->enemies[i].maxHp;
            if (bossPct < 0.0f) bossPct = 0.0f;

            Rectangle barBg = { 340, 92, 600, 26 };
            DrawRectangleRounded(barBg, 0.4f, 6, Fade((Color){ 20, 0, 6, 255 }, 0.85f));
            Color bossCol = (bossPct > 0.66f) ? (Color){ 200, 40, 60, 255 }
                          : (bossPct > 0.33f) ? (Color){ 230, 60, 200, 255 }
                          : (Color){ 255, 90, 40, 255 };
            if (bossPct > 0.0f)
            {
                Rectangle barFill = { barBg.x, barBg.y, barBg.width * bossPct, barBg.height };
                DrawRectangleRounded(barFill, 0.4f, 6, bossCol);
            }
            DrawRectangleRoundedLines(barBg, 0.4f, 6, (Color){ 255, 80, 90, 255 });

            const char *bossName = "SUPERBACTERIA KPC — RESISTENTE";
            Vector2 nSz = MeasureTextEx(font, bossName, 16.0f, 1.0f);
            DrawTextEx(font, bossName, (Vector2){ 640.0f - nSz.x / 2.0f, 70.0f }, 16.0f, 1.0f, (Color){ 255, 120, 130, 255 });
            const char *hpTxt = TextFormat("%d / %d", game->enemies[i].hp, game->enemies[i].maxHp);
            Vector2 hSz = MeasureTextEx(font, hpTxt, 13.0f, 1.0f);
            DrawTextEx(font, hpTxt, (Vector2){ 640.0f - hSz.x / 2.0f, 97.0f }, 13.0f, 1.0f, WHITE);
            break;
        }
    }

    // D. INDICADORES VISUAIS DE BUFFS ATIVOS
    int buffCount = 0;
    
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

    // HUD DA ARMA ATUAL (com barra de cooldown)
    const char *weaponNames[4] = { "1. Lâmina Imunológica", "2. Fuzil Célula-T", "3. Granada Macrófago", "4. Vacina BFG" };
    const float weaponCooldowns[4] = { 0.22f, 0.15f, 1.5f, 5.0f };
    int wpnIdx = game->player.equippedWeapon - 1;
    if (wpnIdx >= 0 && wpnIdx <= 3) {
        Rectangle rWpn = { 20, 130.0f + (float)buffCount * 32.0f, 220, 30 };
        Color wpnCol = WeaponSkinPrimary(game->player.weaponSkinId);
        DrawRectangleRec(rWpn, Fade((Color){ 10, 8, 22, 255 }, 0.75f));
        DrawRectangle((int)rWpn.x, (int)rWpn.y, 4, (int)rWpn.height, wpnCol);
        DrawRectangleLinesEx(rWpn, 1.0f, Fade(wpnCol, 0.25f));

        DrawTextEx(font, weaponNames[wpnIdx], (Vector2){ rWpn.x + 12, rWpn.y + 4 }, 12.0f, 1.0f, WHITE);

        // Barra de cooldown: cheia = pronto para atacar
        float cdMax = weaponCooldowns[wpnIdx];
        float cdPct = 1.0f - (game->player.attackCooldown / cdMax);
        if (cdPct < 0.0f) cdPct = 0.0f;
        if (cdPct > 1.0f) cdPct = 1.0f;
        DrawRectangle((int)rWpn.x + 12, (int)rWpn.y + 21, 196, 4, Fade(BLACK, 0.6f));
        DrawRectangle((int)rWpn.x + 12, (int)rWpn.y + 21, (int)(196 * cdPct), 4,
                      (cdPct >= 1.0f) ? wpnCol : Fade(wpnCol, 0.45f));
        buffCount++;
    }

    // HUD DE POÇÕES
    Rectangle rPot = { 20, 130.0f + (float)buffCount * 32.0f, 220, 26 };
    DrawRectangleRec(rPot, Fade((Color){ 10, 8, 22, 255 }, 0.75f));
    DrawRectangle((int)rPot.x, (int)rPot.y, 4, (int)rPot.height, GREEN);
    DrawRectangleLinesEx(rPot, 1.0f, Fade(GREEN, 0.25f));
    DrawTextEx(font, TextFormat("[E] Poções de Vida: %d", game->player.healthPotions), (Vector2){ rPot.x + 12, rPot.y + 7 }, 12.0f, 1.0f, GREEN);
    buffCount++;

    // E. RADAR ESPACIAL DENTRO DO HUD
    Vector2 radarCenter = { 1195.0f, 85.0f };
    float radarRadius = 65.0f;
    float radarRange = 1200.0f; 
    
    DrawCircleV(radarCenter, radarRadius, Fade((Color){ 10, 8, 22, 255 }, 0.65f));
    
    Rectangle radarFrame = { radarCenter.x - radarRadius - 5, radarCenter.y - radarRadius - 5, radarRadius * 2 + 10, radarRadius * 2 + 10 };
    DrawRectangleLinesEx(radarFrame, 1.0f, Fade(THEME_COLOR_MAIN, 0.3f));
    float len = 6.0f;
    DrawLineEx((Vector2){ radarFrame.x, radarFrame.y }, (Vector2){ radarFrame.x + len, radarFrame.y }, 1.5f, THEME_COLOR_MAIN);
    DrawLineEx((Vector2){ radarFrame.x, radarFrame.y }, (Vector2){ radarFrame.x, radarFrame.y + len }, 1.5f, THEME_COLOR_MAIN);
    DrawLineEx((Vector2){ radarFrame.x + radarFrame.width, radarFrame.y }, (Vector2){ radarFrame.x + radarFrame.width - len, radarFrame.y }, 1.5f, THEME_COLOR_MAIN);
    DrawLineEx((Vector2){ radarFrame.x + radarFrame.width, radarFrame.y }, (Vector2){ radarFrame.x + radarFrame.width, radarFrame.y + len }, 1.5f, THEME_COLOR_MAIN);
    DrawLineEx((Vector2){ radarFrame.x, radarFrame.y + radarFrame.height }, (Vector2){ radarFrame.x + len, radarFrame.y + radarFrame.height }, 1.5f, THEME_COLOR_MAIN);
    DrawLineEx((Vector2){ radarFrame.x, radarFrame.y + radarFrame.height }, (Vector2){ radarFrame.x, radarFrame.y + radarFrame.height - len }, 1.5f, THEME_COLOR_MAIN);
    DrawLineEx((Vector2){ radarFrame.x + radarFrame.width, radarFrame.y + radarFrame.height }, (Vector2){ radarFrame.x + radarFrame.width - len, radarFrame.y + radarFrame.height }, 1.5f, THEME_COLOR_MAIN);
    DrawLineEx((Vector2){ radarFrame.x + radarFrame.width, radarFrame.y + radarFrame.height }, (Vector2){ radarFrame.x + radarFrame.width, radarFrame.y + radarFrame.height - len }, 1.5f, THEME_COLOR_MAIN);

    DrawCircleLines(radarCenter.x, radarCenter.y, radarRadius, THEME_COLOR_MAIN);
    DrawCircleLines(radarCenter.x, radarCenter.y, radarRadius * 0.66f, Fade(THEME_COLOR_MAIN, 0.25f));
    DrawCircleLines(radarCenter.x, radarCenter.y, radarRadius * 0.33f, Fade(THEME_COLOR_MAIN, 0.15f));
    
    DrawLineV((Vector2){ radarCenter.x - radarRadius, radarCenter.y }, (Vector2){ radarCenter.x + radarRadius, radarCenter.y }, Fade(THEME_COLOR_MAIN, 0.2f));
    DrawLineV((Vector2){ radarCenter.x, radarCenter.y - radarRadius }, (Vector2){ radarCenter.x, radarCenter.y + radarRadius }, Fade(THEME_COLOR_MAIN, 0.2f));
    
    float sweepTime = (float)GetTime() * 3.0f;
    Vector2 sweepEnd = {
        radarCenter.x + cosf(sweepTime) * radarRadius,
        radarCenter.y + sinf(sweepTime) * radarRadius
    };
    DrawLineEx(radarCenter, sweepEnd, 1.5f, Fade(THEME_COLOR_MAIN, 0.5f));

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

    float pPulse = 3.0f + sinf((float)GetTime() * 6.0f) * 0.8f;
    DrawCircleV(radarCenter, pPulse, SKYBLUE);
    DrawCircleLines(radarCenter.x, radarCenter.y, pPulse + 2.0f, Fade(SKYBLUE, 0.5f));

    DrawTextEx(font, "BIOSSENSOR", (Vector2){ radarCenter.x - 34.0f, radarCenter.y + radarRadius + 8.0f }, 11.0f, 1.0f, GRAY);
    
    if (game->saveLoaded)
    {
        DrawRectangleRounded((Rectangle){ 490, 670, 300, 30 }, 0.4f, 4, Fade(GREEN, 0.2f));
        DrawRectangleRoundedLines((Rectangle){ 490, 670, 300, 30 }, 0.4f, 4, GREEN);
        
        Vector2 textSz = MeasureTextEx(font, game->notificationMsg, 14.0f, 1.0f);
        DrawTextEx(font, game->notificationMsg, (Vector2){ 490.0f + 150.0f - textSz.x/2.0f, 678.0f }, 14.0f, 1.0f, GREEN);
        
        if (game->timeElapsed > 3.0f) game->saveLoaded = false;
    }
}

void DrawTelaGameplay(GameState *game, Font font, bool drawHUD)
{
    Color bgColor = (Color){ 6, 14, 8, 255 };
    Color gridColor = Fade((Color){0, 100, 40, 255}, 0.15f);
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active && game->enemies[i].tier == TIER_3_BOSS) {
            float hpPercent = (float)game->enemies[i].hp / game->enemies[i].maxHp;
            if (hpPercent < 0.33f) {
                bgColor = (Color){ 30, 5, 5, 255 };
                gridColor = Fade(RED, 0.25f);
            } else if (hpPercent < 0.66f) {
                bgColor = (Color){ 20, 5, 25, 255 };
                gridColor = Fade(MAGENTA, 0.2f);
            }
            break;
        }
    }

    BeginMode2D(game->camera);

    bool isBossFight = false;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active && game->enemies[i].tier == TIER_3_BOSS) {
            isBossFight = true;
            break;
        }
    }
    // Renderiza o fundo do mapa (antes era DrawMapOrganismo)
    DrawRectangleGradientV(0, 0, MAP_WIDTH, MAP_HEIGHT, bgColor, bgColor);
    
    // Grade orgânica de células
    int gridSpacing = 160;
    for (int x = 0; x <= MAP_WIDTH; x += gridSpacing)
        DrawLine(x, 0, x, MAP_HEIGHT, gridColor);
    for (int y = 0; y <= MAP_HEIGHT; y += gridSpacing)
        DrawLine(0, y, MAP_WIDTH, y, gridColor);
    
    // Detalhes orgânicos: círculos de células/vasos
    static const int cellX[] = { 400, 900, 1500, 2200, 3000, 3600, 600, 1200, 1800, 2600, 3200, 700, 1600, 2400, 3500 };
    static const int cellY[] = { 300, 800, 1200, 600, 900, 1400, 2000, 2500, 1700, 2100, 800, 3000, 3200, 2800, 3500 };
    static const int cellR[] = { 60, 45, 80, 55, 70, 40, 65, 50, 75, 45, 85, 55, 40, 70, 60 };
    Color cellColor = isBossFight
        ? Fade((Color){ 120, 0, 40, 255 }, 0.18f)
        : Fade((Color){ 0, 80, 30, 255 }, 0.18f);
    for (int ci = 0; ci < 15; ci++) {
        DrawCircleLines(cellX[ci], cellY[ci], (float)cellR[ci], cellColor);
        DrawCircleLines(cellX[ci], cellY[ci], (float)(cellR[ci] / 2), Fade(cellColor, 0.5f));
    }
    
    // Borda do mapa
    Color borderColor = isBossFight
        ? (Color){ 160, 0, 30, 255 }
        : (Color){ 0, 120, 50, 255 };
    DrawRectangleLinesEx((Rectangle){ 0, 0, MAP_WIDTH, MAP_HEIGHT }, 6.0f, borderColor);


    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (game->powerUps[i].active)
        {
            float pulse = 18.0f + sinf(game->powerUps[i].pulseTimer * 6.0f) * 3.0f;
            Vector2 pos = game->powerUps[i].position;

            Color itemCol = YELLOW;
            if (game->powerUps[i].type == HP_RECOVERY) { itemCol = (Color){ 50, 220, 100, 255 }; }
            else if (game->powerUps[i].type == SPEED_BOOST) { itemCol = THEME_COLOR_MAIN; }
            else if (game->powerUps[i].type == SHIELD) { itemCol = (Color){ 30, 100, 200, 255 }; }
            else if (game->powerUps[i].type == ATTACK_BOOST) { itemCol = (Color){ 255, 60, 60, 255 }; }

            DrawCircleV(pos, pulse + 4.0f, Fade(itemCol, 0.3f));
            DrawPoly(pos, 4, pulse, 0.0f, itemCol);
            DrawPolyLinesEx(pos, 4, pulse, 0.0f, 2.0f, WHITE);

            if (game->powerUps[i].type == HP_RECOVERY) {
                DrawRectangle(pos.x - 2, pos.y - 6, 4, 12, WHITE);
                DrawRectangle(pos.x - 6, pos.y - 2, 12, 4, WHITE);
            } else if (game->powerUps[i].type == SPEED_BOOST) {
                DrawLineEx((Vector2){pos.x - 4, pos.y - 4}, (Vector2){pos.x + 2, pos.y}, 2.0f, WHITE);
                DrawLineEx((Vector2){pos.x - 4, pos.y + 4}, (Vector2){pos.x + 2, pos.y}, 2.0f, WHITE);
                DrawLineEx((Vector2){pos.x - 8, pos.y - 4}, (Vector2){pos.x - 2, pos.y}, 2.0f, WHITE);
                DrawLineEx((Vector2){pos.x - 8, pos.y + 4}, (Vector2){pos.x - 2, pos.y}, 2.0f, WHITE);
            } else if (game->powerUps[i].type == SHIELD) {
                DrawRectangle(pos.x - 5, pos.y - 4, 10, 6, WHITE);
                DrawTriangle((Vector2){pos.x - 5, pos.y + 2}, (Vector2){pos.x, pos.y + 8}, (Vector2){pos.x + 5, pos.y + 2}, WHITE);
            } else if (game->powerUps[i].type == ATTACK_BOOST) {
                DrawRectangle(pos.x - 1, pos.y - 6, 2, 10, WHITE);
                DrawRectangle(pos.x - 4, pos.y + 1, 8, 2, WHITE);
            }
        }
    }

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
                float intensity = (enemy->cooldownTimer / 0.25f) * 6.0f;
                renderPos.x += GetRandomValue(-intensity, intensity);
                renderPos.y += GetRandomValue(-intensity, intensity);
                
                float t = enemy->cooldownTimer / 0.25f;
                squashFactor = 1.0f + sinf(t * PI * 4.0f) * 0.18f;
            }
            else if (enemy->state == DEATH)
            {
                float deathPct = enemy->cooldownTimer / 0.5f;
                if (deathPct < 0.0f) deathPct = 0.0f;
                if (deathPct > 1.0f) deathPct = 1.0f;
                
                scale = deathPct;
                rotation = (1.0f - deathPct) * 360.0f;
                alpha = deathPct;
            }
            
            float currentDestSize = destSize * scale;
            
            DrawEnemyModel(enemy, renderPos, currentDestSize, rotation, squashFactor, alpha);
            
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
                
                DrawStatusIcons(enemy->position, enemy->poisonTimer > 0.0f, enemy->slowTimer > 0.0f, GetTime());
            }
        }
    }

    Color wpnPrim = WeaponSkinPrimary(game->player.weaponSkinId);
    Color wpnSec  = WeaponSkinSecondary(game->player.weaponSkinId);

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
            else if (p->type == PROJ_PLAYER_RIFLE) pCol = wpnPrim;   // skin da arma
            else if (p->type == PROJ_PLAYER_GRENADE) pCol = ORANGE;
            else if (p->type == PROJ_PLAYER_BFG) pCol = wpnPrim;

            if (p->type == PROJ_PLAYER_BFG) {
                srcSize = 30.0f;
                DrawCircleGradient((int)p->position.x, (int)p->position.y, srcSize, pCol, BLANK);
                DrawCircleLines(p->position.x, p->position.y, srcSize, wpnSec);
            } else if (p->type == PROJ_PLAYER_GRENADE) {
                srcSize = 15.0f;
                DrawCircle(p->position.x, p->position.y, srcSize, pCol);
                DrawCircleLines(p->position.x, p->position.y, srcSize, wpnPrim);
            } else if (p->type == PROJ_PLAYER_RIFLE) {
                // Projétil do jogador mais legível: rastro curto + núcleo brilhante
                Vector2 tail = Vector2Subtract(p->position, Vector2Scale(Vector2Normalize(p->velocity), 22.0f));
                DrawLineEx(tail, p->position, 5.0f, Fade(pCol, 0.45f));
                DrawCircle(p->position.x, p->position.y, 9.0f, pCol);
                DrawCircle(p->position.x, p->position.y, 4.0f, wpnSec);
            } else {
                DrawCircle(p->position.x, p->position.y, srcSize, pCol);
                DrawCircleLines(p->position.x, p->position.y, srcSize, WHITE);
            }
        }
    }

    Vector2 pPos = game->player.position;
    float playerSize = 40.0f;

    DrawPlayerHero(game, pPos, playerSize);
    DrawStatusIcons(game->player.position, game->player.poisonTimer > 0.0f, game->player.slowTimer > 0.0f, GetTime());

    if (game->slashAnimTimer > 0.0f)
    {
        float t = 1.0f - (game->slashAnimTimer / 0.22f);
        float currentRadius = t * game->slashAnimRadius;

        DrawCircleLines(game->slashAnimPos.x, game->slashAnimPos.y, currentRadius, Fade(wpnSec, (1.0f - t) * 0.9f));
        DrawCircleLines(game->slashAnimPos.x, game->slashAnimPos.y, currentRadius - 8.0f, Fade(wpnPrim, (1.0f - t) * 0.7f));
        DrawCircle(game->slashAnimPos.x, game->slashAnimPos.y, currentRadius, Fade(wpnPrim, (1.0f - t) * 0.15f));
    }

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (game->particles[i].active)
        {
            float lifePercent = game->particles[i].lifeTime / game->particles[i].maxLifeTime;
            float size = game->particles[i].size * (0.3f + 0.7f * lifePercent);
            DrawRectangleV(
                (Vector2){ game->particles[i].position.x - size, game->particles[i].position.y - size },
                (Vector2){ size * 2.0f, size * 2.0f },
                Fade(game->particles[i].color, lifePercent)
            );
        }
    }

    // Números de dano flutuantes (feedback de combate)
    for (int i = 0; i < MAX_DAMAGE_TEXTS; i++)
    {
        if (game->damageTexts[i].active)
        {
            DamageText *dt = &game->damageTexts[i];
            float alpha = dt->timer / dt->maxTime;
            const char *txt = TextFormat("%d", dt->value);
            Vector2 sz = MeasureTextEx(font, txt, 20.0f, 1.0f);
            Vector2 pos = { dt->position.x - sz.x / 2.0f, dt->position.y };
            DrawTextEx(font, txt, (Vector2){ pos.x + 1, pos.y + 1 }, 20.0f, 1.0f, Fade(BLACK, alpha * 0.8f));
            DrawTextEx(font, txt, pos, 20.0f, 1.0f, Fade(dt->color, alpha));
        }
    }

    EndMode2D();

    float hpPct = (float)game->player.hp / game->player.maxHp;
    if (hpPct < 0.25f && g_assets.shdLowHP.id != 0) {
        BeginShaderMode(g_assets.shdLowHP);
        float time = (float)GetTime();
        float res[2] = { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
        SetShaderValue(g_assets.shdLowHP, g_assets.shdLowHPTimeLoc, &time, SHADER_UNIFORM_FLOAT);
        SetShaderValue(g_assets.shdLowHP, g_assets.shdLowHPResLoc, res, SHADER_UNIFORM_VEC2);
        
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
        EndShaderMode();
    }

    if (drawHUD)
    {
        DrawHUD(game, font);
    }
}

void DrawTelaTutorial(GameState *game, Font font)
{
    ClearBackground(BLACK);
    BeginMode2D(game->camera);

    DrawMapSeringa(font, game->tutorialStep, (float)GetTime(), game->injectionTimer);

    Vector2 pPos = game->player.position;
    float ps = 38.0f;
    DrawPlayerHero(game, pPos, ps);

    if (game->slashAnimTimer > 0.0f)
    {
        float t = 1.0f - (game->slashAnimTimer / 0.22f);
        float cr = t * game->slashAnimRadius;
        DrawCircleLines(game->slashAnimPos.x, game->slashAnimPos.y, cr, Fade(WHITE, (1.0f - t) * 0.9f));
        DrawCircle(game->slashAnimPos.x, game->slashAnimPos.y, cr, Fade(THEME_COLOR_MAIN, (1.0f - t) * 0.15f));
    }

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
                float intensity = (e->cooldownTimer / 0.25f) * 6.0f;
                renderPos.x += GetRandomValue(-intensity, intensity);
                renderPos.y += GetRandomValue(-intensity, intensity);
                
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
                rotationAngle = (1.0f - deathPct) * PI * 2.0f;
                alpha = deathPct;
            }
            
            float currentEs = es * scale;
            Color bactCol = (e->state == HURT) ? WHITE : (Color){ 50, 200, 80, 255 };
            Color bactLineCol = (Color){ 20, 140, 50, 255 };
            
            bactCol = Fade(bactCol, alpha);
            bactLineCol = Fade(bactLineCol, alpha);
            
            DrawEllipse(renderPos.x, renderPos.y, currentEs * squashX, currentEs * squashY, bactCol);
            DrawEllipseLines(renderPos.x, renderPos.y, currentEs * squashX, currentEs * squashY, bactLineCol);
            
            float c = cosf(rotationAngle);
            float s = sinf(rotationAngle);
            
            Vector2 f1Start = { -currentEs * squashX * c, -currentEs * squashX * s };
            Vector2 f1End   = { (-currentEs * squashX - 15.0f) * c - 10.0f * s, (-currentEs * squashX - 15.0f) * s + 10.0f * c };
            
            Vector2 f2Start = { currentEs * squashX * c, currentEs * squashX * s };
            Vector2 f2End   = { (currentEs * squashX + 15.0f) * c - (-10.0f) * s, (currentEs * squashX + 15.0f) * s + (-10.0f) * c };
            
            DrawLineV(Vector2Add(renderPos, f1Start), Vector2Add(renderPos, f1End), bactLineCol);
            DrawLineV(Vector2Add(renderPos, f2Start), Vector2Add(renderPos, f2End), bactLineCol);
            
            if (e->state != DEATH)
            {
                float barW = 70.0f;
                DrawRectangle(e->position.x - barW/2, e->position.y - es - 14, barW, 8, Fade(RED, 0.5f));
                float hpPct = (float)e->hp / e->maxHp;
                DrawRectangle(e->position.x - barW/2, e->position.y - es - 14, barW * hpPct, 8, (Color){ 50, 200, 80, 255 });
                DrawRectangleLinesEx((Rectangle){ e->position.x - barW/2, e->position.y - es - 14, barW, 8 }, 1.0f, BLACK);
                DrawStatusIcons(e->position, e->poisonTimer > 0.0f, e->slowTimer > 0.0f, GetTime());
            }
        }
    }

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (game->particles[i].active)
        {
            float lp = game->particles[i].lifeTime / game->particles[i].maxLifeTime;
            float size = game->particles[i].size * (0.3f + 0.7f * lp);
            DrawRectangleV((Vector2){ game->particles[i].position.x - size, game->particles[i].position.y - size }, (Vector2){ size * 2.0f, size * 2.0f }, Fade(game->particles[i].color, lp));
        }
    }

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

    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (game->powerUps[i].active)
        {
            float pulse = 18.0f + sinf(game->powerUps[i].pulseTimer * 6.0f) * 3.0f;
            Vector2 pos = game->powerUps[i].position;

            Color itemCol = YELLOW;
            if (game->powerUps[i].type == HP_RECOVERY) { itemCol = (Color){ 50, 220, 100, 255 }; }
            else if (game->powerUps[i].type == SPEED_BOOST) { itemCol = THEME_COLOR_MAIN; }
            else if (game->powerUps[i].type == SHIELD) { itemCol = (Color){ 30, 100, 200, 255 }; }
            else if (game->powerUps[i].type == ATTACK_BOOST) { itemCol = (Color){ 255, 60, 60, 255 }; }

            DrawCircleV(pos, pulse + 4.0f, Fade(itemCol, 0.3f));
            DrawPoly(pos, 4, pulse, 0.0f, itemCol);
            DrawPolyLinesEx(pos, 4, pulse, 0.0f, 2.0f, WHITE);

            if (game->powerUps[i].type == HP_RECOVERY) {
                DrawRectangle(pos.x - 2, pos.y - 6, 4, 12, WHITE);
                DrawRectangle(pos.x - 6, pos.y - 2, 12, 4, WHITE);
            } else if (game->powerUps[i].type == SPEED_BOOST) {
                DrawLineEx((Vector2){pos.x - 4, pos.y - 4}, (Vector2){pos.x + 2, pos.y}, 2.0f, WHITE);
                DrawLineEx((Vector2){pos.x - 4, pos.y + 4}, (Vector2){pos.x + 2, pos.y}, 2.0f, WHITE);
                DrawLineEx((Vector2){pos.x - 8, pos.y - 4}, (Vector2){pos.x - 2, pos.y}, 2.0f, WHITE);
                DrawLineEx((Vector2){pos.x - 8, pos.y + 4}, (Vector2){pos.x - 2, pos.y}, 2.0f, WHITE);
            } else if (game->powerUps[i].type == SHIELD) {
                DrawRectangle(pos.x - 5, pos.y - 4, 10, 6, WHITE);
                DrawTriangle((Vector2){pos.x - 5, pos.y + 2}, (Vector2){pos.x, pos.y + 8}, (Vector2){pos.x + 5, pos.y + 2}, WHITE);
            } else if (game->powerUps[i].type == ATTACK_BOOST) {
                DrawRectangle(pos.x - 1, pos.y - 6, 2, 10, WHITE);
                DrawRectangle(pos.x - 4, pos.y + 1, 8, 2, WHITE);
            }
        }
    }

    // Números de dano flutuantes (também no tutorial)
    for (int i = 0; i < MAX_DAMAGE_TEXTS; i++)
    {
        if (game->damageTexts[i].active)
        {
            DamageText *dt = &game->damageTexts[i];
            float alpha = dt->timer / dt->maxTime;
            const char *txt = TextFormat("%d", dt->value);
            Vector2 sz = MeasureTextEx(font, txt, 20.0f, 1.0f);
            Vector2 dpos = { dt->position.x - sz.x / 2.0f, dt->position.y };
            DrawTextEx(font, txt, (Vector2){ dpos.x + 1, dpos.y + 1 }, 20.0f, 1.0f, Fade(BLACK, alpha * 0.8f));
            DrawTextEx(font, txt, dpos, 20.0f, 1.0f, Fade(dt->color, alpha));
        }
    }

    EndMode2D();

    float hpPct = (float)game->player.hp / game->player.maxHp;
    if (hpPct < 0.25f && g_assets.shdLowHP.id != 0) {
        BeginShaderMode(g_assets.shdLowHP);
        float time = (float)GetTime();
        float res[2] = { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
        SetShaderValue(g_assets.shdLowHP, g_assets.shdLowHPTimeLoc, &time, SHADER_UNIFORM_FLOAT);
        SetShaderValue(g_assets.shdLowHP, g_assets.shdLowHPResLoc, res, SHADER_UNIFORM_VEC2);

        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
        EndShaderMode();
    }

    char stepStr[32];
    sprintf(stepStr, "PASSO %d/3", game->tutorialStep + 1);
    DrawSciFiBox((Rectangle){ 1050, 20, 210, 45 }, (Color){ 0, 200, 100, 255 });
    Vector2 stepSz = MeasureTextEx(font, stepStr, 18.0f, 1.0f);
    DrawTextEx(font, stepStr, (Vector2){ 1155.0f - stepSz.x / 2.0f, 32.0f }, 18.0f, 1.0f, (Color){ 0, 220, 120, 255 });

    if (game->tutorialDialog.active)
    {
        DrawRectangleRounded((Rectangle){ 80, 580, 1120, 120 }, 0.08f, 6, Fade((Color){ 6, 18, 12, 255 }, 0.92f));
        DrawRectangleRoundedLines((Rectangle){ 80, 580, 1120, 120 }, 0.08f, 6, (Color){ 0, 200, 100, 255 });

        DrawCircle(120, 640, 22, (Color){ 0, 150, 200, 255 });
        DrawCircleLines(120, 640, 22, THEME_COLOR_MAIN);
        DrawTextEx(font, "Ab", (Vector2){ 108, 630 }, 16.0f, 1.0f, WHITE);

        const char *fullL1 = "";
        const char *fullL2 = "";
        const char *fullL3 = "";
        GetTutorialDialogText(game->tutorialStep, game->tutorialDialog.page, &fullL1, &fullL2, &fullL3);

        char l1Draw[128] = { 0 };
        char l2Draw[128] = { 0 };
        char l3Draw[128] = { 0 };

        int charLimit = game->tutorialDialog.charShown;
        int len1 = strlen(fullL1);
        int len2 = strlen(fullL2);
        int len3 = strlen(fullL3);

        if (charLimit <= len1)
        {
            strncpy(l1Draw, fullL1, charLimit);
        }
        else
        {
            strcpy(l1Draw, fullL1);
            int rem = charLimit - len1;
            if (rem <= len2)
            {
                strncpy(l2Draw, fullL2, rem);
            }
            else
            {
                strcpy(l2Draw, fullL2);
                int rem2 = rem - len2;
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
