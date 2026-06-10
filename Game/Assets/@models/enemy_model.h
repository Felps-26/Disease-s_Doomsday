#ifndef ENEMY_MODEL_H
#define ENEMY_MODEL_H

#include "raylib.h"
#include "../../include/game.h"

// Função para desenhar o modelo dos inimigos
void DrawEnemyModel(Enemy *enemy, Vector2 renderPos, float destSize, float rotation, float squashFactor, float alpha);

#endif // ENEMY_MODEL_H
