#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <stdbool.h>

typedef struct Player
{
    char name[16];        // Nome do jogador
    Vector2 position;
    float speed;
    int hp;
    int maxHp;
    int score;
    int level;
    int xp;
    int xpNeeded;
    int attackPower;
    int susPoints; // Pontos para upgrades (Pontos do SUS)
    
    // Timers de Buffs e Debuffs (ativos se > 0)
    float speedTimer;
    float shieldTimer;
    float attackBoostTimer;
    float poisonTimer; // Dano ao longo do tempo
    float slowTimer;   // Redução de velocidade
    
    // Combate
    float attackCooldown;  // Tempo até o próximo ataque
    float damageCooldown;  // I-frames: invencibilidade após tomar dano (0.5s)
    
    // Animação
    int facingDir; // 1 para Direita, -1 para Esquerda
    bool isMoving;
    float squashX;
    float squashY;
    Vector2 trail[10];
    int trailIndex;
    
    // Armas
    int equippedWeapon; // 1 = Lâmina, 2 = Fuzil, 3 = Granada, 4 = Vacina BFG

    int healthPotions; // Poções de vida

    // Skins (visuais, sem efeito de balanceamento)
    int skinId;       // 0 = Padrão, 1 = Médica, 2 = Infectada
    int weaponSkinId; // 0 = Padrão, 1 = Plasma, 2 = Tóxica

} Player;

// Forward declaration of GameState
struct GameState;

// Player logic
void PlayerAttack(struct GameState *game, Vector2 worldMousePos);

#endif // PLAYER_H
