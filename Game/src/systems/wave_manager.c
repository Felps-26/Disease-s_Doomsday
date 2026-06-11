#include "wave_manager.h"
#include "../../include/gameplay.h"
#include "raymath.h"

// Posiciona um ponto de spawn longe do jogador (distância mínima de 450 px)
static Vector2 PickSpawnFarFromPlayer(GameState *game)
{
    Vector2 spawnPos;
    int tries = 0;
    do
    {
        spawnPos.x = (float)GetRandomValue(100, MAP_WIDTH - 100);
        spawnPos.y = (float)GetRandomValue(100, MAP_HEIGHT - 100);
        tries++;
    } while (Vector2DistanceSqr(game->player.position, spawnPos) < 450.0f * 450.0f && tries < 32);
    return spawnPos;
}

// Configura o chefe final (Superbactéria KPC) num índice específico
static void ConfigureBoss(GameState *game, int idx)
{
    Enemy *b = &game->enemies[idx];
    b->position = PickSpawnFarFromPlayer(game);
    b->active = true;
    b->type = 2;            // KPC / Superbactéria
    b->tier = TIER_3_BOSS;
    b->maxHp = 1400 + game->wave * 100;
    b->hp = b->maxHp;
    b->speed = 70.0f;
    b->isRanged = true;
    b->state = IDLE;
    b->patrolTarget = b->position;
    b->patrolTimer = 3.0f;
    b->cooldownTimer = 1.5f;
    b->chargeTimer = 0.0f;
    b->poisonTimer = 0.0f;
    b->poisonAccum = 0.0f;
    b->slowTimer = 0.0f;
    b->isTutorialEnemy = false;
}

void StartNextWave(GameState *game)
{
    // ------------------------------------------------------------------
    // ONDA FINAL (5): Confronto garantido com o Chefe (Superbactéria KPC)
    // Antes o chefe só aparecia por acaso (chance < 10%), tornando o clímax
    // imprevisível. Agora a última onda é sempre uma batalha de chefe com um
    // grupo curado de lacaios — desafiador, porém justo.
    // ------------------------------------------------------------------
    bool bossWave = (game->wave >= 5);

    // Aumenta quantidade a cada onda
    int numEnemies = bossWave ? 13 : (8 + game->wave * 4); // 1 chefe + 12 lacaios na final
    if (numEnemies > MAX_ENEMIES) numEnemies = MAX_ENEMIES;

    game->enemiesRemaining = numEnemies;

    int startIdx = 0;
    if (bossWave)
    {
        ConfigureBoss(game, 0); // O chefe é sempre o inimigo 0
        startIdx = 1;
    }

    // Spawna os lacaios/inimigos comuns longe do jogador
    for (int i = startIdx; i < numEnemies; i++)
    {
        Vector2 spawnPos = PickSpawnFarFromPlayer(game);

        game->enemies[i].position = spawnPos;
        game->enemies[i].active = true;
        game->enemies[i].poisonAccum = 0.0f;

        // Determina tipo e dificuldade do inimigo.
        // Na onda do chefe, os lacaios pulam o tipo KPC pesado (já há o chefe).
        int randVal = GetRandomValue(0, bossWave ? 89 : 100);

        if (randVal < 40 || game->wave == 1)
        {
            // Tipo 0: SARS-CoV-2 (Equilibrado, Melee)
            game->enemies[i].type = 0;
            game->enemies[i].tier = TIER_1;
            game->enemies[i].maxHp = 30 + game->wave * 10;
            game->enemies[i].hp = game->enemies[i].maxHp;
            game->enemies[i].speed = 140.0f + GetRandomValue(-10, 10);
            game->enemies[i].isRanged = false;
        }
        else if (randVal < 60)
        {
            // Tipo 1: Vírus da Dengue (Rápido, Atirador)
            game->enemies[i].type = 1;
            game->enemies[i].tier = TIER_2;
            game->enemies[i].isRanged = true;
            game->enemies[i].maxHp = 20 + game->wave * 5;
            game->enemies[i].hp = game->enemies[i].maxHp;
            game->enemies[i].speed = 220.0f + GetRandomValue(-15, 15);
        }
        else if (randVal < 75)
        {
            // Tipo 3: Trypanosoma cruzi (Chagas) (Muito rápido, Melee, Frágil)
            game->enemies[i].type = 3;
            game->enemies[i].tier = TIER_2;
            game->enemies[i].maxHp = 15 + game->wave * 5;
            game->enemies[i].hp = game->enemies[i].maxHp;
            game->enemies[i].speed = 280.0f + GetRandomValue(-20, 20);
            game->enemies[i].isRanged = false;
        }
        else if (randVal < 90)
        {
            // Tipo 4: Mycobacterium tuberculosis (Atirador Pesado)
            game->enemies[i].type = 4;
            game->enemies[i].tier = TIER_3;
            game->enemies[i].maxHp = 100 + game->wave * 20;
            game->enemies[i].hp = game->enemies[i].maxHp;
            game->enemies[i].speed = 80.0f + GetRandomValue(-5, 5);
            game->enemies[i].isRanged = true;
        }
        else
        {
            // Tipo 2: KPC (Superbactéria, Elite, Lento e Tank).
            // O chefe propriamente dito é criado por ConfigureBoss(); aqui é
            // apenas uma versão elite que aparece a partir da onda 3.
            game->enemies[i].type = 2;
            game->enemies[i].tier = TIER_3;
            game->enemies[i].maxHp = 200 + game->wave * 40;
            game->enemies[i].hp = game->enemies[i].maxHp;
            game->enemies[i].speed = 60.0f + GetRandomValue(-5, 5);
            game->enemies[i].isRanged = true;
        }

        game->enemies[i].state = IDLE;
        game->enemies[i].patrolTarget = spawnPos;
        game->enemies[i].patrolTimer = (float)GetRandomValue(2, 5);
        game->enemies[i].cooldownTimer = (float)GetRandomValue(1, 3); // Stagger inicial entre inimigos
        game->enemies[i].chargeTimer = 0.0f;
        game->enemies[i].poisonTimer = 0.0f;
        game->enemies[i].slowTimer = 0.0f;
        game->enemies[i].isTutorialEnemy = false;
    }

    // Garante que existam alguns power-ups espalhados no mapa no início da onda
    int powerUpsCount = 4 + game->wave;
    if (powerUpsCount > 10) powerUpsCount = 10;
    for (int i = 0; i < powerUpsCount; i++)
    {
        Vector2 itemPos = {
            (float)GetRandomValue(200, MAP_WIDTH - 200),
            (float)GetRandomValue(200, MAP_HEIGHT - 200)
        };
        SpawnPowerUpAt(game, itemPos, -1); // Tipo aleatório
    }

    // Partículas azuis de invocação de nova onda
    for (int p = 0; p < 30; p++)
    {
        Vector2 vel = { (float)GetRandomValue(-150, 150), (float)GetRandomValue(-150, 150) };
        SpawnParticle(game, game->player.position, vel, SKYBLUE, 6.0f, 1.2f);
    }
}
