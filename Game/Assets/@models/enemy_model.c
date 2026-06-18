#include "enemy_model.h"
#include "../../include/sprite_manager.h"
#include "raymath.h"
#include <math.h>

// ============================================================================
// MODELO: INIMIGOS
// ----------------------------------------------------------------------------
// Pipeline de sprites (Fase 1): se houver um PNG do tipo do inimigo em
// Assets/Sprites/Enemies/..., ele é desenhado; caso contrário, recai no desenho
// procedural abaixo. Os tipos seguem os #define ETYPE_* em enemy.h.
// ============================================================================

// Mapeia o tipo do inimigo para o SpriteID correspondente (-1 = sem mapeamento).
static SpriteID EnemySpriteFor(Enemy *enemy)
{
    switch (enemy->type)
    {
        case ETYPE_BACT_MELEE:  return SPR_BACT_MELEE;
        case ETYPE_BACT_RANGED: return SPR_BACT_RANGED;
        case ETYPE_KPC:         return (enemy->tier == TIER_3_BOSS) ? SPR_BACT_BOSS : SPR_BACT_RANGED;
        case ETYPE_VIRUS_MELEE:  return SPR_VIRUS_MELEE;
        case ETYPE_VIRUS_RANGED: return SPR_VIRUS_RANGED;
        case ETYPE_VIRUS_BOSS:   return SPR_VIRUS_BOSS;
        default:                 return (SpriteID)-1;
    }
}

void DrawEnemyModel(Enemy *enemy, Vector2 renderPos, float destSize, float rotation, float squashFactor, float alpha)
{
    float currentDestSize = destSize * squashFactor;

    // ---- Pipeline de sprites: usa o PNG se disponível ----
    SpriteID spr = EnemySpriteFor(enemy);
    if (spr != (SpriteID)-1 && SpriteAvailable(spr))
    {
        Color tint = (enemy->state == HURT) ? Fade(WHITE, alpha) : Fade(WHITE, alpha);
        DrawSpriteCentered(spr, renderPos, (Vector2){ currentDestSize * 2.4f, currentDestSize * 2.4f }, rotation, tint);
        return;
    }

    Color enemyCol = RED;
    if (enemy->state == HURT) {
        enemyCol = WHITE;
    } else {
        if (enemy->type == ETYPE_SARS)             enemyCol = (Color){ 140, 50, 200, 255 }; // Roxo (SARS-CoV-2)
        else if (enemy->type == ETYPE_DENGUE_OLD)  enemyCol = (Color){ 80, 80, 80, 255 };   // Cinza escuro (Dengue legado)
        else if (enemy->type == ETYPE_KPC)         enemyCol = (Color){ 50, 200, 80, 255 };  // Verde toxico (KPC)
        else if (enemy->type == ETYPE_CHAGAS)      enemyCol = (Color){ 30, 100, 220, 255 }; // Azul (Chagas)
        else if (enemy->type == ETYPE_TB)          enemyCol = (Color){ 180, 60, 40, 255 };  // Marrom (Tuberculose)
        else if (enemy->type == ETYPE_BACT_MELEE)  enemyCol = (Color){ 90, 200, 120, 255 }; // Verde (cocos)
        else if (enemy->type == ETYPE_BACT_RANGED) enemyCol = (Color){ 170, 200, 70, 255 }; // Oliva (bacilo)
        else if (enemy->type == ETYPE_VIRUS_MELEE) enemyCol = (Color){ 230, 90, 90, 255 };  // Vermelho (dengue)
        else if (enemy->type == ETYPE_VIRUS_RANGED)enemyCol = (Color){ 240, 160, 60, 255 }; // Laranja (influenza)
        else if (enemy->type == ETYPE_VIRUS_BOSS)  enemyCol = (Color){ 200, 60, 160, 255 }; // Magenta (chefe viral)
        else                                       enemyCol = RED;
    }

    enemyCol = Fade(enemyCol, alpha);
    Color edgeCol = Fade(WHITE, alpha);

    // Tipo 0: Vírus (Círculo espinhoso)
    if (enemy->type == ETYPE_SARS) {
        DrawPoly(renderPos, 8, currentDestSize, rotation + GetTime()*10.0f, enemyCol);
        DrawPolyLinesEx(renderPos, 8, currentDestSize, rotation + GetTime()*10.0f, 3.0f, edgeCol);
        for (int i=0; i<8; i++) {
            float angle = (rotation + GetTime()*10.0f + i * 45.0f) * DEG2RAD;
            Vector2 end = { renderPos.x + cosf(angle) * currentDestSize * 1.3f, renderPos.y + sinf(angle) * currentDestSize * 1.3f };
            DrawLineEx(renderPos, end, 2.0f, enemyCol);
            DrawCircleV(end, 4.0f, edgeCol);
        }
    }
    // Tipo 1: Dengue / Mosquito (legado)
    else if (enemy->type == ETYPE_DENGUE_OLD) {
        DrawPoly(renderPos, 3, currentDestSize, rotation + GetTime()*20.0f, enemyCol);
        DrawPolyLines(renderPos, 3, currentDestSize, rotation + GetTime()*20.0f, edgeCol);
        DrawCircleV(renderPos, currentDestSize * 0.4f, RED);
    }
    // Tipo 2: KPC / Superbactéria (Tanque Hexagonal)
    else if (enemy->type == ETYPE_KPC) {
        int sides = 6;
        float rotSpeed = 2.0f;
        DrawPoly(renderPos, sides, currentDestSize, rotation + GetTime()*rotSpeed, enemyCol);
        DrawPolyLinesEx(renderPos, sides, currentDestSize, rotation + GetTime()*rotSpeed, 4.0f, edgeCol);
        DrawCircleV(renderPos, currentDestSize * 0.5f, BLACK);
    }
    // Tipo 3: Trypanosoma cruzi / Chagas (legado)
    else if (enemy->type == ETYPE_CHAGAS) {
        DrawPoly(renderPos, 4, currentDestSize, rotation + GetTime()*25.0f, enemyCol);
        DrawPoly(renderPos, 4, currentDestSize*0.7f, -rotation - GetTime()*25.0f, SKYBLUE);
    }
    // Tipo 4: Tuberculose (Pentágono)
    else if (enemy->type == ETYPE_TB) {
        DrawPoly(renderPos, 5, currentDestSize, rotation + GetTime()*5.0f, enemyCol);
        DrawPolyLines(renderPos, 5, currentDestSize, rotation + GetTime()*5.0f, edgeCol);
    }
    // Tipo 5: Bactéria corpo a corpo (cocos — aglomerado de esferas)
    else if (enemy->type == ETYPE_BACT_MELEE) {
        DrawCircleV(renderPos, currentDestSize, enemyCol);
        DrawCircleLines((int)renderPos.x, (int)renderPos.y, currentDestSize, edgeCol);
        // pequenos cocos satélites girando
        for (int i = 0; i < 4; i++) {
            float a = (rotation + GetTime() * 60.0f + i * 90.0f) * DEG2RAD;
            Vector2 c = { renderPos.x + cosf(a) * currentDestSize * 0.55f, renderPos.y + sinf(a) * currentDestSize * 0.55f };
            DrawCircleV(c, currentDestSize * 0.35f, Fade(WHITE, alpha * 0.5f));
        }
    }
    // Tipo 6: Bactéria à distância (bacilo — bastonete)
    else if (enemy->type == ETYPE_BACT_RANGED) {
        float ang = rotation * DEG2RAD;
        Vector2 dir = { cosf(ang), sinf(ang) };
        Vector2 a = { renderPos.x - dir.x * currentDestSize, renderPos.y - dir.y * currentDestSize };
        Vector2 b = { renderPos.x + dir.x * currentDestSize, renderPos.y + dir.y * currentDestSize };
        DrawLineEx(a, b, currentDestSize * 0.9f, enemyCol);
        DrawCircleV(a, currentDestSize * 0.45f, enemyCol);
        DrawCircleV(b, currentDestSize * 0.45f, enemyCol);
        DrawCircleV(renderPos, currentDestSize * 0.3f, Fade(BLACK, alpha * 0.6f));
    }
    // Tipos 7/8/9: Vírus (círculo espinhoso) — com tamanho/cor por tipo
    else if (enemy->type == ETYPE_VIRUS_MELEE || enemy->type == ETYPE_VIRUS_RANGED || enemy->type == ETYPE_VIRUS_BOSS) {
        int spikes = (enemy->type == ETYPE_VIRUS_BOSS) ? 12 : 9;
        DrawPoly(renderPos, spikes, currentDestSize, rotation + GetTime()*8.0f, enemyCol);
        DrawPolyLinesEx(renderPos, spikes, currentDestSize, rotation + GetTime()*8.0f, 3.0f, edgeCol);
        for (int i = 0; i < spikes; i++) {
            float angle = (rotation + GetTime()*8.0f + i * (360.0f / spikes)) * DEG2RAD;
            Vector2 end = { renderPos.x + cosf(angle) * currentDestSize * 1.35f, renderPos.y + sinf(angle) * currentDestSize * 1.35f };
            DrawLineEx(renderPos, end, 2.5f, enemyCol);
            DrawCircleV(end, currentDestSize * 0.12f, edgeCol);
        }
        // núcleo de material genético (RNA)
        DrawCircleV(renderPos, currentDestSize * 0.45f, Fade((Color){ 60, 20, 30, 255 }, alpha));
    }
}
