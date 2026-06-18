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

// ----------------------------------------------------------------------------
// TIPOS DE PATÓGENO (campo Enemy.type)
// 0..4: tipos legados (compatibilidade de save). 5+: tipos dos Mundos da
// expansão. O comportamento de IA segue principalmente isRanged/tier; o `type`
// define o visual (enemy_model.c / sprites) e algumas particularidades.
// ----------------------------------------------------------------------------
#define ETYPE_SARS        0   // legado: SARS-CoV-2 (vírus, melee equilibrado)
#define ETYPE_DENGUE_OLD  1   // legado: dengue (vírus, ranged rápido)
#define ETYPE_KPC         2   // bactéria: Superbactéria KPC (tanque/chefe Mundo 1)
#define ETYPE_CHAGAS      3   // legado: Trypanosoma cruzi (protozoário, melee)
#define ETYPE_TB          4   // bactéria: Mycobacterium tuberculosis (ranged pesado)
// --- Mundo 1 (Bactérias) ---
#define ETYPE_BACT_MELEE  5   // bactéria corpo a corpo (cocos/pneumonia)
#define ETYPE_BACT_RANGED 6   // bactéria à distância (bacilo atirador)
// --- Mundo 2 (Vírus, com escudo de capsídeo) ---
#define ETYPE_VIRUS_MELEE  7  // vírus corpo a corpo com escudo (tema dengue)
#define ETYPE_VIRUS_RANGED 8  // vírus à distância com escudo (tema influenza)
#define ETYPE_VIRUS_BOSS   9  // chefe viral com escudo reforçado (Mundo 2, onda 5)

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

    // ---- Escudo de capsídeo (Mundo 2 — Vírus) ----
    // Enquanto shieldActive, o inimigo NÃO recebe dano de vida: o dano vai
    // primeiro para shieldHp. Só após quebrar o capsídeo (shieldHp<=0) ele fica
    // vulnerável. Inimigos sem escudo deixam shieldActive=false.
    bool  shieldActive;   // true = capsídeo intacto, protege a vida
    int   shieldHp;       // pontos de vida do escudo
    int   shieldMaxHp;    // capacidade máxima do escudo
    float shieldHitFlash; // brilho ao receber dano no escudo
} Enemy;

#endif // ENEMY_H
