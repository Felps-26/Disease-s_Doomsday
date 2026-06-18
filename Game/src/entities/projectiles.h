#ifndef PROJECTILES_H
#define PROJECTILES_H

#include "raylib.h"
#include <stdbool.h>

typedef enum ProjectileType
{
    PROJ_ACID_ARC,          // Ácido bacteriano (Tier 1)
    PROJ_BULLET_SPREAD,     // Picada espalhada do Aedes aegypti (Tier 2)
    PROJ_VOID_BOLT,         // Toxina viral concentrada (Tier 3 / Elite)
    PROJ_BOSS_BULLET,       // Projétil da Superbactéria Resistente (Boss)
    PROJ_PLAYER_RIFLE,      // Fuzil de Células T (legado / genérico)
    PROJ_PLAYER_GRENADE,    // Granada Macrófago
    PROJ_PLAYER_BFG,        // Vacina BFG
    PROJ_PLAYER_PHAGE,      // Rifle de Bacteriófagos (Mundo 1): bônus vs. bactérias
    PROJ_PLAYER_VACCINE     // Rifle de Vacina (Mundo 2): bônus vs. vírus / escudo
} ProjectileType;

typedef struct Projectile
{
    Vector2 position;
    Vector2 velocity;
    bool active;
    ProjectileType type;
    int damage;
    Rectangle hitbox;
    bool isPlayerProjectile;
    float lifeTime; // Util para granada e bfg
    
} Projectile;

// Forward declaration of GameState
struct GameState;

void SpawnProjectile(struct GameState *game, Vector2 pos, Vector2 target, ProjectileType type, int dmg);

#endif // PROJECTILES_H
