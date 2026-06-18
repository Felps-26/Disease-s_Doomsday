#include "projectiles.h"
#include "../../include/game.h"
#include "raymath.h"

void SpawnProjectile(GameState *game, Vector2 pos, Vector2 target, ProjectileType type, int dmg)
{
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (!game->projectiles[i].active)
        {
            game->projectiles[i].position = pos;
            Vector2 dir = Vector2Normalize(Vector2Subtract(target, pos));
            float speed = 300.0f;
            if (type == PROJ_ACID_ARC)        speed = 220.0f;
            if (type == PROJ_BULLET_SPREAD)   speed = 350.0f;
            if (type == PROJ_VOID_BOLT)       speed = 400.0f;
            if (type == PROJ_BOSS_BULLET)     speed = 300.0f;
            if (type == PROJ_PLAYER_RIFLE)    speed = 700.0f; // Bala rápida
            if (type == PROJ_PLAYER_GRENADE)  speed = 350.0f; // Granada mais lenta
            if (type == PROJ_PLAYER_BFG)      speed = 280.0f; // BFG pesada
            if (type == PROJ_PLAYER_PHAGE)    speed = 680.0f; // Bacteriófago (rápido)
            if (type == PROJ_PLAYER_VACCINE)  speed = 680.0f; // Dose de vacina (rápida)

            game->projectiles[i].velocity = Vector2Scale(dir, speed);
            game->projectiles[i].active = true;
            game->projectiles[i].type = type;
            game->projectiles[i].damage = dmg;
            game->projectiles[i].isPlayerProjectile = (type == PROJ_PLAYER_RIFLE || type == PROJ_PLAYER_GRENADE ||
                                                       type == PROJ_PLAYER_BFG || type == PROJ_PLAYER_PHAGE ||
                                                       type == PROJ_PLAYER_VACCINE);
            game->projectiles[i].lifeTime = (type == PROJ_PLAYER_GRENADE) ? 1.2f : ((type == PROJ_PLAYER_BFG) ? 3.0f : 8.0f);
            game->projectiles[i].hitbox = (Rectangle){ pos.x - 10, pos.y - 10, 20, 20 };
            break;
        }
    }
}
