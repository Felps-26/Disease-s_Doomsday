#ifndef SPATIAL_GRID_H
#define SPATIAL_GRID_H

#include "../include/game.h"

#define GRID_CELL_SIZE 400
#define GRID_COLS (MAP_WIDTH / GRID_CELL_SIZE + 1)
#define GRID_ROWS (MAP_HEIGHT / GRID_CELL_SIZE + 1)

typedef struct {
    int count;
    int enemyIndices[MAX_ENEMIES]; // Em um cenário real seria uma lista encadeada dinâmica, mas para MAX_ENEMIES=60, isso é mais rápido de alocar e iterar.
} GridCell;

void ClearGrid(void);
void UpdateGrid(GameState *game);
// Preenche resultIndices com inimigos no raio informado e retorna a quantidade
int GetEnemiesInRadius(GameState *game, Vector2 center, float radius, int *resultIndices);
int GetEnemiesInRect(GameState *game, Rectangle rect, int *resultIndices);

#endif
