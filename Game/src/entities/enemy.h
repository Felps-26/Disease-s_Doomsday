#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include <stdbool.h>

typedef enum EnemyState
{
    IDLE,
    AGGRO,
    ATTACK,
    HURT,
    DEATH
} EnemyState;

typedef enum EnemyTier
{
    TIER_1,
    TIER_2,
    TIER_3,
    TIER_3_BOSS
} EnemyTier;

typedef struct Enemy
{
    Vector2 position;
    float speed;
    int hp;
    int maxHp;
    EnemyState state;
    Vector2 patrolTarget;
    float patrolTimer;
    // Tipos de Patógenos:
    // 0 = Vírus: SARS-CoV-2 (Equilibrado)
    // 1 = Vírus da Dengue (Mosquito Aedes aegypti - Rápido, Atirador)
    // 2 = Bactéria: KPC (Superbactéria - Lento, Muito HP, Dano alto)
    // 3 = Protozoário: Trypanosoma cruzi (Rápido, corpo-a-corpo)
    // 4 = Bactéria: Mycobacterium tuberculosis (Tuberculose - Atirador pesado)
    int type;
    EnemyTier tier;
    bool active;     // Ativo/vivo no jogo
    bool isTutorialEnemy; // Inimigo de treino do tutorial (não ataca)
    
    // Ranged
    float cooldownTimer;
    float chargeTimer;
    bool isRanged;
    
    // Status effects
    float poisonTimer;
    float poisonAccum; // Acumulador fracionário do dano de veneno (hp é int)
    float slowTimer;
} Enemy;

#endif // ENEMY_H
