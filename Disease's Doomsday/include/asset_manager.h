#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "raylib.h"
#include <stdbool.h>

// Estrutura global para manter os assets do jogo
typedef struct GameAssets {
    Font font;
    Music musicMain;
    Music musicB;
    
    // SFX de combate
    Sound sfxSwordSlice;
    Sound sfxRifleShoot;
    Sound sfxGrenadeLaunch;
    Sound sfxGrenadeExplode;
    Sound sfxBFGShoot;
    Sound sfxBFGDamage;
    Sound sfxHeroHurt;
    Sound sfxBacteriaHurt;
    Sound sfxVirusHurt;
    Sound sfxBacteriaBossHurt;
    Sound sfxVirusBossHurt;
    Sound sfxEnemyShoot;

    // Interface
    Sound sfxMenuClick;
    Sound sfxMenuHover;
    Sound sfxQuizHover;

    // Feedback legado sem arquivo dedicado no pacote novo
    Sound sfxPickup;
    Sound sfxDeath;
    
    // Shaders
    Shader biologicalShader;
    Shader shdLowHP;
    int shdLowHPTimeLoc;
    int shdLowHPResLoc;
    bool shaderLoaded;

} GameAssets;

extern GameAssets g_assets;

// Inicializa o sistema de áudio e carrega todos os assets do jogo
void LoadGameAssets(void);

// Descarrega os assets e fecha o dispositivo de áudio
void UnloadGameAssets(void);

// Roteamento central: evita que armas e famílias de patógeno voltem a usar um
// efeito genérico. `enemyType` e `enemyTier` recebem os valores de Enemy.
void PlayWeaponAttackSfx(int weapon);
void PlayEnemyDamageSfx(int enemyType, int enemyTier);

// Aplica o ganho global de efeitos preservando o volume relativo de cada SFX.
void ApplySfxVolume(float volume);

#endif // ASSET_MANAGER_H
