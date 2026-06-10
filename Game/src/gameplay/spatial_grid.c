#include "../../include/spatial_grid.h"
#include <math.h>
#include "raymath.h"

static GridCell enemyGrid[GRID_COLS][GRID_ROWS];

void ClearGrid(void) {
    for (int x = 0; x < GRID_COLS; x++) {
        for (int y = 0; y < GRID_ROWS; y++) {
            enemyGrid[x][y].count = 0;
        }
    }
}

void UpdateGrid(GameState *game) {
    ClearGrid();
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active && game->enemies[i].state != DEATH) {
            int cx = (int)(game->enemies[i].position.x / GRID_CELL_SIZE);
            int cy = (int)(game->enemies[i].position.y / GRID_CELL_SIZE);
            
            if (cx < 0) cx = 0;
            if (cx >= GRID_COLS) cx = GRID_COLS - 1;
            if (cy < 0) cy = 0;
            if (cy >= GRID_ROWS) cy = GRID_ROWS - 1;
            
            int count = enemyGrid[cx][cy].count;
            if (count < MAX_ENEMIES) {
                enemyGrid[cx][cy].enemyIndices[count] = i;
                enemyGrid[cx][cy].count++;
            }
        }
    }
}

int GetEnemiesInRadius(GameState *game, Vector2 center, float radius, int *resultIndices) {
    int minX = (int)((center.x - radius) / GRID_CELL_SIZE);
    int maxX = (int)((center.x + radius) / GRID_CELL_SIZE);
    int minY = (int)((center.y - radius) / GRID_CELL_SIZE);
    int maxY = (int)((center.y + radius) / GRID_CELL_SIZE);

    if (minX < 0) minX = 0;
    if (maxX >= GRID_COLS) maxX = GRID_COLS - 1;
    if (minY < 0) minY = 0;
    if (maxY >= GRID_ROWS) maxY = GRID_ROWS - 1;

    int totalCount = 0;
    // Evita duplicatas verificando se já inserimos (embora as células sejam exclusivas)
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            GridCell *cell = &enemyGrid[x][y];
            for (int i = 0; i < cell->count; i++) {
                int idx = cell->enemyIndices[i];
                float dist = Vector2Distance(center, game->enemies[idx].position);
                if (dist <= radius) {
                    resultIndices[totalCount++] = idx;
                }
            }
        }
    }
    return totalCount;
}

int GetEnemiesInRect(GameState *game, Rectangle rect, int *resultIndices) {
    int minX = (int)(rect.x / GRID_CELL_SIZE);
    int maxX = (int)((rect.x + rect.width) / GRID_CELL_SIZE);
    int minY = (int)(rect.y / GRID_CELL_SIZE);
    int maxY = (int)((rect.y + rect.height) / GRID_CELL_SIZE);

    if (minX < 0) minX = 0;
    if (maxX >= GRID_COLS) maxX = GRID_COLS - 1;
    if (minY < 0) minY = 0;
    if (maxY >= GRID_ROWS) maxY = GRID_ROWS - 1;

    int totalCount = 0;
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            GridCell *cell = &enemyGrid[x][y];
            for (int i = 0; i < cell->count; i++) {
                int idx = cell->enemyIndices[i];
                Vector2 pos = game->enemies[idx].position;
                float radius = (game->enemies[idx].type == 2) ? 35.0f : 20.0f;
                // Colisão simples rect x circle expandido
                if (pos.x + radius >= rect.x && pos.x - radius <= rect.x + rect.width &&
                    pos.y + radius >= rect.y && pos.y - radius <= rect.y + rect.height) {
                    resultIndices[totalCount++] = idx;
                }
            }
        }
    }
    return totalCount;
}
