#include "player.h"
#include "../../include/game.h"
#include "../../include/gameplay.h"
#include "../../include/spatial_grid.h"
#include "../../include/asset_manager.h"
#include "../../src/entities/projectiles.h"
#include "../../Assets/@models/weapons_model.h"
#include "raymath.h"

void PlayerAttack(GameState *game, Vector2 worldMousePos)
{
    if (game->player.attackCooldown > 0.0f) return;

    // BUGFIX: garante arma válida (após load de save antigo, equippedWeapon
    // podia ficar 0 e o ataque silenciosamente não fazia nada)
    if (game->player.equippedWeapon < 1 || game->player.equippedWeapon > WEAPON_COUNT)
        game->player.equippedWeapon = 1;

    int wpn = game->player.equippedWeapon;
    int danoBase = game->player.attackPower;
    if (game->player.attackBoostTimer > 0.0f) danoBase *= 2; // Buff de ataque dobra dano

    Color skinPrim = WeaponSkinPrimary(game->player.weaponSkinId);
    Color skinSec  = WeaponSkinSecondary(game->player.weaponSkinId);

    if (wpn == 1 || wpn == WEAPON_BIOBLADE) {
        // Ambas as armas melee (Espada-Seringa e Lâmina Bioelétrica) usam o mesmo
        // golpe em área 360°; a Lâmina adiciona o bônus anti-capsídeo abaixo.
        // Define cooldown base
        game->player.attackCooldown = 0.22f;
        PlayWeaponAttackSfx(wpn);

        // Configura animação de ataque (Slash)
        game->slashAnimTimer = 0.22f;
        game->slashAnimPos = game->player.position;
        game->slashAnimRadius = 140.0f;

        // Efeitos visuais: explosão de partículas de slash (cores da skin da arma)
        SpawnParticleExplosion(game, game->player.position, skinSec, 12, 100.0f, 250.0f, 4.0f, 0.4f);
        SpawnParticleExplosion(game, game->player.position, skinPrim, 8, 150.0f, 300.0f, 3.5f, 0.35f);

        // Câmera dá uma leve chacoalhada no ataque
        game->screenShake = 0.25f;

        // BUGFIX: o golpe agora acerta em 360 graus, exatamente como o efeito
        // visual circular sugere. Antes era um cone de +-90 graus na direção
        // do mouse — atacando com ESPAÇO com o mouse "do lado errado", o
        // jogador errava a bactéria mesmo colado nela.
        int collIndices[MAX_ENEMIES];
        int collCount = GetEnemiesInRadius(game, game->player.position, game->slashAnimRadius, collIndices);

        for (int k = 0; k < collCount; k++)
        {
            int i = collIndices[k];
            if (!game->enemies[i].active || game->enemies[i].state == DEATH) continue;

            // Acertou! Causa dano (com proteções anti-melt para chefes/escudo)
            int danoTotal = 15 + danoBase;
            // Lâmina Bioelétrica (arma 5): descarrega corrente no capsídeo — dano
            // muito maior contra o ESCUDO viral, quebrando-o bem mais rápido. Vale
            // em QUALQUER Mundo (a arma é desbloqueada por abates na campanha).
            if (wpn == WEAPON_BIOBLADE && game->enemies[i].shieldActive && game->enemies[i].shieldHp > 0)
                danoTotal *= 3;
            int applied = ApplyPlayerDamageToEnemy(game, &game->enemies[i], danoTotal, false);
            if (applied <= 0) continue; // bloqueado pelo escudo do chefe
            PlayEnemyDamageSfx(game->enemies[i].type, game->enemies[i].tier);
            SpawnDamageText(game, game->enemies[i].position, applied, skinSec);

            // Empurrão (Knockback) na direção oposta ao jogador (chefes resistem)
            float knockMag = (game->enemies[i].tier == TIER_3_BOSS) ? 8.0f
                           : (game->enemies[i].tier == TIER_MINIBOSS) ? 22.0f : 55.0f;
            Vector2 knockbackDir = Vector2Subtract(game->enemies[i].position, game->player.position);
            if (knockbackDir.x == 0.0f && knockbackDir.y == 0.0f) knockbackDir = (Vector2){ 1.0f, 0.0f };
            knockbackDir = Vector2Normalize(knockbackDir);

            // Empurra o inimigo a uma distância segura
            game->enemies[i].position = Vector2Add(game->enemies[i].position, Vector2Scale(knockbackDir, knockMag));

            // Partículas de sangue/dano no local do inimigo
            Color hitColor = (game->enemies[i].type == 2) ? MAROON : RED;
            SpawnParticleExplosion(game, game->enemies[i].position, hitColor, 15, 80.0f, 180.0f, 3.0f, 0.5f);

            // Se o inimigo morreu
            if (game->enemies[i].hp <= 0)
            {
                game->enemies[i].hp = 0;
                game->enemies[i].state = DEATH;
                game->enemies[i].cooldownTimer = 0.5f; // Duração da animação de morte

                // Partículas de morte e ganho de xp
                SpawnParticleExplosion(game, game->enemies[i].position, GOLD, 20, 50.0f, 150.0f, 4.0f, 0.7f);

                // Durante o tutorial NÃO contabilizamos onda nem score real
                if (game->inTutorial)
                {
                    // Apenas efeito visual de vitória na seringa
                    SpawnParticleExplosion(game, game->enemies[i].position, (Color){0, 220, 120, 255}, 15, 60.0f, 140.0f, 4.0f, 0.7f);
                    continue;
                }

                // --- Lógica normal de jogo (fora do tutorial) ---
                // Recompensa centralizada (XP/score/contador/drop) idêntica à das
                // demais armas. A conclusão da onda é verificada no fim do
                // UpdateGameplay (vale para todas as armas).
                RegisterEnemyKill(game, &game->enemies[i]);
            }
            else
            {
                // Se o inimigo não morreu, entra em estado de ferimento (stun/flash)
                game->enemies[i].state = HURT;
                game->enemies[i].cooldownTimer = 0.25f; // flash/stun de 0.25s
            }
        }
        // O golpe em área também destroi os Núcleos de Infecção do escudo do chefe
        HitInfectionCores(game, game->player.position, game->slashAnimRadius, 15 + danoBase);
    }
    else if (wpn == 2) {
        // Cadência (Fase 5): 0.28s — antes 0.15s permitia spray contínuo. Agora
        // o alcance (~1050px) e a cadência limitam a "limpeza" sem mirar.
        game->player.attackCooldown = 0.28f;
        PlayWeaponAttackSfx(wpn);
        game->screenShake = 0.1f;
        // Arma de projétil do Mundo atual: Rifle de Bacteriófagos (Mundo 1, bônus
        // vs. bactérias) ou Rifle de Vacina (Mundo 2, bônus vs. vírus).
        ProjectileType rt = (game->currentWorld == WORLD_VIRUS) ? PROJ_PLAYER_VACCINE : PROJ_PLAYER_PHAGE;
        SpawnProjectile(game, game->player.position, worldMousePos, rt, 8 + danoBase);
    }
    else if (wpn == 3) {
        game->player.attackCooldown = 1.5f;
        PlayWeaponAttackSfx(wpn);
        SpawnProjectile(game, game->player.position, worldMousePos, PROJ_PLAYER_GRENADE, 40 + danoBase);
    }
    else if (wpn == 4) {
        game->player.attackCooldown = 5.0f;
        PlayWeaponAttackSfx(wpn);
        game->screenShake = 0.8f;
        SpawnProjectile(game, game->player.position, worldMousePos, PROJ_PLAYER_BFG, 100 + danoBase);
    }
}

// ============================================================================
// DADOS DAS ARMAS (fonte única usada por HUD, Arsenal e Tutorial)
// ============================================================================
// Mundo atual para nomear as armas temáticas (definido por SetWeaponWorld).
static int s_weaponWorld = WORLD_BACTERIA;
void SetWeaponWorld(int world)
{
    s_weaponWorld = (world == WORLD_VIRUS) ? WORLD_VIRUS : WORLD_BACTERIA;
    SetWeaponModelWorld(s_weaponWorld); // legado: modelo melee do slot 1 já não depende do Mundo
}

WeaponInfo GetWeaponInfo(int weapon)
{
    bool virus = (s_weaponWorld == WORLD_VIRUS);
    switch (weapon)
    {
        case 2:
            // Arma de projétil temática por Mundo (mesmas estatísticas-base).
            if (virus) return (WeaponInfo){
                "Rifle de Vacina", "Doses de imunizacao em linha reta.",
                "+60% de dano contra VIRUS", "Imuniza: eficaz contra virus (vacinas treinam o sistema imune).",
                8, "Rapida (0.28s)", 0.28f, 2, 2, (Color){ 120, 200, 255, 255 }, 1050.0f };
            return (WeaponInfo){
                "Rifle de Bacteriofagos", "Dispara bacteriofagos em linha reta.",
                "+60% de dano contra BACTERIAS", "Bacteriofagos sao virus que infectam bacterias; otimo no Mundo 1.",
                8, "Rapida (0.28s)", 0.28f, 2, 2, (Color){ 120, 255, 160, 255 }, 1050.0f };
        case 3: return (WeaponInfo){
            "Desestabilizador de RNA", "Granada que ataca o RNA em area.",
            "Dano em area + VENENO (ignora capsideo)", "Controle de grupos; forte vs. bacterias e virus.",
            40, "Lenta", 1.5f, 3, 3, (Color){ 255, 140, 40, 255 } };
        case 4: return (WeaponInfo){
            "BFG Imunologico", "Projetil pesado que ATRAVESSA inimigos.",
            "Perfurante (atravessa todos)", "Limpa fileiras inteiras; guarde para hordas e chefe.",
            100, "Muito lenta", 5.0f, 4, 4, (Color){ 120, 255, 160, 255 } };
        case WEAPON_BIOBLADE:
            // Arma melee desbloqueável (30 abates): a antiga "Escalpelizador
            // Estatico" promovida a arma própria, disponível na campanha inteira.
            // Descarrega corrente no capsídeo -> quebra escudos em qualquer Mundo.
            return (WeaponInfo){
                "Lamina Bioeletrica", "Golpe em 360 graus que descarrega corrente no capsideo.",
                "Quebra ESCUDOS (capsideo) muito mais rapido", "Desbloqueia com 30 abates; alterne com a tecla 5.",
                15, "Rapida", 0.22f, 1, WEAPON_BIOBLADE, (Color){ 180, 120, 255, 255 } };
        case 1:
        default:
            // Arma corpo a corpo inicial — sempre disponível, em qualquer Mundo.
            return (WeaponInfo){
                "Espada-Seringa", "Golpe corpo a corpo em 360 graus.",
                "Acerta tudo ao redor + empurrao", "Defesa pessoal; segura inimigos colados em voce.",
                15, "Rapida", 0.22f, 1, 1, (Color){ 0, 229, 255, 255 } };
    }
}

// Desbloqueio unificado (troca em jogo + arsenal). Armas 1-4 seguem a progressão
// de nível (maxWeaponUnlocked); a Lâmina Bioelétrica (5) abre por ABATES.
bool WeaponUnlocked(GameState *game, int weapon)
{
    if (weapon == WEAPON_BIOBLADE)
        return game->totalEnemiesKilled >= BIOBLADE_UNLOCK_KILLS;
    return (weapon >= 1 && weapon <= game->maxWeaponUnlocked);
}

int WeaponUnlockLevel(int weapon)
{
    return GetWeaponInfo(weapon).unlockLevel;
}

const char *WeaponName(int weapon)
{
    return GetWeaponInfo(weapon).name;
}
