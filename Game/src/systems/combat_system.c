#include "combat_system.h"
#include "../../include/gameplay.h"
#include "../../include/asset_manager.h"
#include "raymath.h"

void HandlePlayerEnemyCollision(GameState *game, Enemy *enemy)
{
    // I-FRAMES: Se o jogador tomou dano recentemente, ignora este hit
    if (game->player.damageCooldown > 0.0f) {
        // Mesmo com I-frames, ainda repele o inimigo
        Vector2 pushDir = Vector2Subtract(enemy->position, game->player.position);
        if (pushDir.x == 0.0f && pushDir.y == 0.0f) pushDir = (Vector2){ 0.0f, 1.0f };
        enemy->position = Vector2Add(enemy->position, Vector2Scale(Vector2Normalize(pushDir), 55.0f));
        return;
    }

    // Se o escudo estiver ativo, absorve o dano e cria faíscas azuis
    if (game->player.shieldTimer > 0.0f)
    {
        SpawnParticleExplosion(game, game->player.position, SKYBLUE, 10, 80.0f, 150.0f, 3.0f, 0.4f);
        game->player.damageCooldown = 0.3f; // Curto cooldown mesmo com escudo
    }
    else
    {
        // Escala o dano base pela onda
        int dmgBase = 8 + game->wave * 2;
        if (enemy->type == 1) {
            dmgBase += 4; // Aedes
            game->player.poisonTimer = 3.0f;
        }
        if (enemy->type == 2) {
            dmgBase += 14; // KPC
            game->player.slowTimer = 2.0f;
        }
        if (enemy->tier == TIER_MINIBOSS) dmgBase += 10;
        if (enemy->tier == TIER_3_BOSS)   dmgBase += 18;

        // Dificuldade: multiplica o dano recebido
        float dmul = (game->diff.enemyDamageMul <= 0.01f) ? 1.0f : game->diff.enemyDamageMul;
        dmgBase = (int)(dmgBase * dmul);
        // Máscara Hospitalar: reduz o dano recebido em 40% enquanto ativa.
        if (game->player.maskTimer > 0.0f) dmgBase = (int)(dmgBase * 0.6f);
        if (dmgBase < 1) dmgBase = 1;

        game->player.hp -= dmgBase;
        game->player.damageCooldown = 0.5f; // 0.5s de invencibilidade
        game->screenShake = 0.4f;
        game->hurtFlashTimer = 0.35f; // flash vermelho de dano
        game->player.squashX = 1.4f;
        game->player.squashY = 0.6f;

        SpawnParticleExplosion(game, game->player.position, RED, 12, 50.0f, 130.0f, 3.5f, 0.5f);

        if (game->player.hp <= 0)
        {
            PlaySound(g_assets.sfxDeath);
            game->player.hp = 0;
            game->player.damageCooldown = 0.0f;
            game->currentScreen = SCREEN_GAMEOVER;
            return;
        } else {
            PlaySound(g_assets.sfxHurt);
        }
    }

    // Repele inimigo
    Vector2 pushDir = Vector2Subtract(enemy->position, game->player.position);
    if (pushDir.x == 0.0f && pushDir.y == 0.0f) pushDir = (Vector2){ 0.0f, 1.0f };
    pushDir = Vector2Normalize(pushDir);
    enemy->position = Vector2Add(enemy->position, Vector2Scale(pushDir, 55.0f));
    enemy->state = HURT;
    enemy->cooldownTimer = 0.5f;
}

void HandleProjectileCollision(GameState *game, Projectile *proj)
{
    proj->active = false;
    
    // I-FRAMES: Se invencível, ignora dano do projétil
    if (game->player.damageCooldown > 0.0f) return;
    
    if (game->player.shieldTimer > 0.0f) {
        SpawnParticleExplosion(game, game->player.position, SKYBLUE, 10, 80.0f, 150.0f, 3.0f, 0.4f);
        game->player.damageCooldown = 0.2f;
    } else {
        // Dano escala levemente com a onda e com a dificuldade
        float dmul = (game->diff.enemyDamageMul <= 0.01f) ? 1.0f : game->diff.enemyDamageMul;
        int dmg = (int)((proj->damage + game->wave) * dmul);
        // Máscara Hospitalar: reduz o dano recebido em 40% enquanto ativa.
        if (game->player.maskTimer > 0.0f) dmg = (int)(dmg * 0.6f);
        if (dmg < 1) dmg = 1;
        game->player.hp -= dmg;
        game->player.squashX = 1.3f;
        game->player.squashY = 0.7f;
        game->player.damageCooldown = 0.35f; // I-frames após projétil
        game->hurtFlashTimer = 0.35f; // flash vermelho de dano
        
        if (proj->type == PROJ_ACID_ARC) {
            game->player.poisonTimer = 3.0f;
        } else if (proj->type == PROJ_VOID_BOLT) {
            game->player.slowTimer = 2.0f;
        }

        game->screenShake = 0.3f;
        SpawnParticleExplosion(game, game->player.position, RED, 10, 50.0f, 100.0f, 3.0f, 0.5f);
        if (game->player.hp <= 0) {
            PlaySound(g_assets.sfxDeath);
            game->player.hp = 0;
            game->player.damageCooldown = 0.0f;
            game->currentScreen = SCREEN_GAMEOVER;
            return;
        } else {
            PlaySound(g_assets.sfxHurt);
        }
    }
}
