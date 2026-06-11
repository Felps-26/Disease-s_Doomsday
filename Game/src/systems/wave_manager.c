#include "wave_manager.h"
#include "../../include/gameplay.h"
#include "raymath.h"
#include <math.h>

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
    b->flankSign = 0.0f;
    b->fleeTimer = 0.0f;
    b->isEscort = false;
    b->aiPhase = 0;
    b->summonTimer = 6.0f;
    b->hitCooldown = 0.0f;
}

// Configura um MINI CHEFE da onda (entre o elite e o chefe final), escalando
// proceduralmente conforme a fase aumenta. NÃO morre num tiro de arma forte.
static void ConfigureMiniBoss(GameState *game, int idx)
{
    Enemy *m = &game->enemies[idx];
    m->position = PickSpawnFarFromPlayer(game);
    m->active = true;
    m->type = 2;                 // visual de superbactéria (tanque atirador)
    m->tier = TIER_MINIBOSS;
    m->maxHp = 300 + game->wave * 220;   // wave1=520 ... wave4=1180
    m->hp = m->maxHp;
    m->speed = 90.0f + game->wave * 6.0f;
    m->isRanged = true;
    m->state = IDLE;
    m->patrolTarget = m->position;
    m->patrolTimer = 3.0f;
    m->cooldownTimer = 1.2f;
    m->chargeTimer = 0.0f;
    m->poisonTimer = 0.0f; m->poisonAccum = 0.0f; m->slowTimer = 0.0f;
    m->isTutorialEnemy = false;
    m->flankSign = (GetRandomValue(0, 1) ? 1.0f : -1.0f);
    m->fleeTimer = 0.0f; m->isEscort = false; m->aiPhase = 0;
    m->summonTimer = 8.0f;        // invoca lacaios ocasionalmente
    m->hitCooldown = 0.0f;
}

// Spawna um lacaio de escolta numa posição (perto do mini chefe/chefe).
static void SpawnEscortMinion(GameState *game, Vector2 pos)
{
    for (int k = 0; k < MAX_ENEMIES; k++)
    {
        if (game->enemies[k].active) continue;
        Enemy *e = &game->enemies[k];
        e->position = pos;
        e->active = true;
        e->type = (GetRandomValue(0, 1) ? 1 : 3);   // Dengue ou Chagas
        e->tier = TIER_2;
        e->isRanged = (e->type == 1);
        e->maxHp = 18 + game->wave * 5;
        e->hp = e->maxHp;
        e->speed = (e->type == 1) ? 220.0f : 270.0f;
        e->state = IDLE;
        e->patrolTarget = pos;
        e->patrolTimer = 3.0f;
        e->cooldownTimer = 1.5f;
        e->chargeTimer = 0.0f;
        e->poisonTimer = 0.0f; e->poisonAccum = 0.0f; e->slowTimer = 0.0f;
        e->isTutorialEnemy = false;
        e->flankSign = (GetRandomValue(0, 1) ? 1.0f : -1.0f);
        e->fleeTimer = 0.0f; e->isEscort = true; e->aiPhase = 0; e->summonTimer = 0.0f;
        e->hitCooldown = 0.0f;
        game->enemiesRemaining++;
        return;
    }
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

    // Spawna os lacaios/inimigos comuns. Na onda do chefe eles nascem em anel
    // ao redor do chefe, formando uma escolta que protege/distrai.
    Vector2 bossPos = bossWave ? game->enemies[0].position : (Vector2){ 0, 0 };
    for (int i = startIdx; i < numEnemies; i++)
    {
        Vector2 spawnPos;
        bool escort = false;
        if (bossWave)
        {
            float ang = (float)(i - startIdx) / (float)(numEnemies - startIdx) * 2.0f * PI;
            float ring = 170.0f + (float)((i - startIdx) % 3) * 55.0f;
            spawnPos.x = bossPos.x + cosf(ang) * ring;
            spawnPos.y = bossPos.y + sinf(ang) * ring;
            // Mantém dentro do mapa
            if (spawnPos.x < 80.0f) spawnPos.x = 80.0f;
            if (spawnPos.x > MAP_WIDTH - 80.0f) spawnPos.x = MAP_WIDTH - 80.0f;
            if (spawnPos.y < 80.0f) spawnPos.y = 80.0f;
            if (spawnPos.y > MAP_HEIGHT - 80.0f) spawnPos.y = MAP_HEIGHT - 80.0f;
            escort = true;
        }
        else
        {
            spawnPos = PickSpawnFarFromPlayer(game);
        }

        game->enemies[i].position = spawnPos;
        game->enemies[i].active = true;
        game->enemies[i].poisonAccum = 0.0f;
        // Inicializa campos de IA avançada
        game->enemies[i].flankSign = (GetRandomValue(0, 1) == 0) ? -1.0f : 1.0f;
        game->enemies[i].fleeTimer = 0.0f;
        game->enemies[i].isEscort = escort;
        game->enemies[i].aiPhase = 0;
        game->enemies[i].summonTimer = 0.0f;

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
        game->enemies[i].hitCooldown = 0.0f;
    }

    // ------------------------------------------------------------------
    // MINI CHEFE: cada onda comum (1-4) ganha um mini chefe como evento
    // principal, acompanhado de uma pequena escolta. A onda 5 já tem o
    // CHEFE final, então não recebe mini chefe.
    // ------------------------------------------------------------------
    if (!bossWave)
    {
        for (int k = 0; k < MAX_ENEMIES; k++)
        {
            if (!game->enemies[k].active)
            {
                ConfigureMiniBoss(game, k);
                game->enemiesRemaining++;
                // Escolta ao redor do mini chefe (aumenta por onda)
                int escorts = 2 + game->wave / 2;
                for (int e = 0; e < escorts; e++)
                {
                    float ang = (float)e / (float)escorts * 2.0f * PI;
                    Vector2 ep = { game->enemies[k].position.x + cosf(ang) * 140.0f,
                                   game->enemies[k].position.y + sinf(ang) * 140.0f };
                    if (ep.x < 80.0f) ep.x = 80.0f;
                    if (ep.x > MAP_WIDTH - 80.0f) ep.x = MAP_WIDTH - 80.0f;
                    if (ep.y < 80.0f) ep.y = 80.0f;
                    if (ep.y > MAP_HEIGHT - 80.0f) ep.y = MAP_HEIGHT - 80.0f;
                    SpawnEscortMinion(game, ep);
                }
                break;
            }
        }
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

    // ------------------------------------------------------------------
    // DIFICULDADE: escala a vida de TODOS os inimigos recém-spawnados.
    // (dano e velocidade são aplicados em tempo real na IA/combate.)
    // ------------------------------------------------------------------
    float hmul = game->diff.enemyHealthMul;
    if (hmul <= 0.01f) hmul = 1.0f; // segurança caso a dificuldade não tenha sido aplicada
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (game->enemies[i].active && !game->enemies[i].isTutorialEnemy)
        {
            int hp = (int)(game->enemies[i].maxHp * hmul);
            if (hp < 1) hp = 1;
            game->enemies[i].maxHp = hp;
            game->enemies[i].hp = hp;
        }
    }

    // Partículas azuis de invocação de nova onda
    for (int p = 0; p < 30; p++)
    {
        Vector2 vel = { (float)GetRandomValue(-150, 150), (float)GetRandomValue(-150, 150) };
        SpawnParticle(game, game->player.position, vel, SKYBLUE, 6.0f, 1.2f);
    }

    // Banner de aviso da nova horda / chefe.
    if (bossWave)
    {
        ShowBanner(game, "CHEFE: SUPERBACTERIA KPC",
                   "Cercada por lacaios! Use a Granada e a Vacina BFG.",
                   (Color){ 255, 70, 90, 255 }, 4.0f);
        game->screenShake = 0.8f;
    }
    else
    {
        const char *sub = (game->wave == 4)
            ? "MINI CHEFE presente! E a proxima horda traz o CHEFE final!"
            : "Um MINI CHEFE lidera esta horda. Elimine todos para avancar.";
        ShowBanner(game, TextFormat("HORDA %d / 5", game->wave), sub,
                   (Color){ 0, 229, 255, 255 }, 3.4f);
    }
}
