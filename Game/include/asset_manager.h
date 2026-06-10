#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "raylib.h"
#include <stdbool.h>

// Estrutura global para manter os assets do jogo
typedef struct GameAssets {
    Font font;
    Music musicMain;
    
    // SFX
    Sound sfxAttack;
    Sound sfxHurt;
    Sound sfxPickup;
    Sound sfxDeath;
    Sound sfxEnemyHurt;
    Sound sfxEnemyShoot;
    
    // Shaders
    Shader biologicalShader;
    bool shaderLoaded;

} GameAssets;

extern GameAssets g_assets;

// Inicializa o sistema de áudio e carrega todos os assets do jogo
void LoadGameAssets(void);

// Descarrega os assets e fecha o dispositivo de áudio
void UnloadGameAssets(void);

#endif // ASSET_MANAGER_H
