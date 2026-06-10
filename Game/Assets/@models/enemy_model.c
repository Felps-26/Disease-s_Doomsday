#include "enemy_model.h"
#include "raymath.h"
#include <math.h>

// ============================================================================
// MODELO: INIMIGOS
// ============================================================================
void DrawEnemyModel(Enemy *enemy, Vector2 renderPos, float destSize, float rotation, float squashFactor, float alpha)
{
    float currentDestSize = destSize * squashFactor;

    Color enemyCol = RED;
    if (enemy->state == HURT) {
        enemyCol = WHITE;
    } else {
        if (enemy->tier == TIER_1)      enemyCol = (Color){ 50, 200, 80, 255 };
        else if (enemy->tier == TIER_2) enemyCol = (Color){ 255, 160, 30, 255 };
        else if (enemy->tier == TIER_3) enemyCol = (Color){ 140, 0, 200, 255 };
        else                            enemyCol = (Color){ 180, 0, 60, 255 };
    }
    
    enemyCol = Fade(enemyCol, alpha);
    Color edgeCol = Fade(WHITE, alpha);

    // Tipo 1: Quadrado (Bactéria Simples)
    if (enemy->tier == TIER_1) {
        DrawPoly(renderPos, 4, currentDestSize, rotation + GetTime()*10.0f, enemyCol);
        DrawPolyLines(renderPos, 4, currentDestSize, rotation + GetTime()*10.0f, edgeCol);
    } 
    // Tipo 2: Triângulo (Rápido)
    else if (enemy->tier == TIER_2) {
        DrawPoly(renderPos, 3, currentDestSize, rotation + GetTime()*15.0f, enemyCol);
        DrawPolyLines(renderPos, 3, currentDestSize, rotation + GetTime()*15.0f, edgeCol);
    } 
    // Tipo 3: Pentágono (Elite) e BOSS
    else {
        int sides = (enemy->tier == TIER_3) ? 5 : 8;
        float rotSpeed = (enemy->tier == TIER_3) ? 5.0f : 2.0f;
        DrawPoly(renderPos, sides, currentDestSize, rotation + GetTime()*rotSpeed, enemyCol);
        DrawPolyLines(renderPos, sides, currentDestSize, rotation + GetTime()*rotSpeed, edgeCol);
    }
}
