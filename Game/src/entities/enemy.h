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
    TIER_3_BOSS,
    TIER_MINIBOSS   // Mini chefe de cada onda (entre o elite e o chefe final)
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

    // ---- IA avançada ----
    float flankSign;   // -1 ou +1: lado preferido para cercar o jogador (melee)
    float fleeTimer;   // > 0 = recuando (inimigo frágil com pouca vida)
    bool  isEscort;    // true = lacaio escolta do chefe (protege/orbita o boss)
    int   aiPhase;     // fase de comportamento do CHEFE (0,1,2) p/ detectar transições
    float summonTimer; // cronômetro do chefe para invocar lacaios
    float hitCooldown; // i-frames de acerto p/ chefes/minichefes (evita melt por arma perfurante)

    // ---- Percepção e esquiva (sistema de dificuldade) ----
    Vector2 lastKnownPlayerPos; // última posição vista do jogador
    float aggroMemory;          // tempo restante perseguindo a última posição conhecida
    float dodgeCooldown;        // cooldown entre esquivas de projétil
} Enemy;

#endif // ENEMY_H
