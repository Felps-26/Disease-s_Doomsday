#include "wave_manager.h"
#include "../../include/gameplay.h"
#include "../../Assets/Maps/map_body.h"
#include "raymath.h"
#include <math.h>

// Posiciona um ponto de spawn DENTRO do corpo, longe do jogador (>= 450 px).
static Vector2 PickSpawnFarFromPlayer(GameState *game)
{
    return MapBody_RandomPointInside(game->player.position, 450.0f);
}

// Configura o chefe final (Superbactéria KPC) num índice específico
static void ConfigureBoss(GameState *game, int idx)
{
    Enemy *b = &game->enemies[idx];
    b->position = PickSpawnFarFromPlayer(game);
    b->active = true;
    // Visual do chefe conforme o Mundo: Superbactéria KPC (Mundo 1) ou chefe
    // viral de capsídeo reforçado (Mundo 2). O "escudo do chefe" usa o sistema
    // de Núcleos de Infecção (InfectionCore) já existente para ambos.
    b->type = (game->currentWorld == WORLD_VIRUS) ? ETYPE_VIRUS_BOSS : ETYPE_KPC;
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
    b->shieldActive = false; b->shieldHp = 0; b->shieldMaxHp = 0; b->shieldHitFlash = 0.0f;
}

// Configura um MINI CHEFE da onda (entre o elite e o chefe final), escalando
// proceduralmente conforme a fase aumenta. NÃO morre num tiro de arma forte.
static void ConfigureMiniBoss(GameState *game, int idx)
{
    Enemy *m = &game->enemies[idx];
    m->position = PickSpawnFarFromPlayer(game);
    m->active = true;
    // Mini chefe temático por Mundo: superbactéria tanque (Mundo 1) ou vírus
    // atirador reforçado com escudo de capsídeo (Mundo 2).
    bool virus = (game->currentWorld == WORLD_VIRUS);
    m->type = virus ? ETYPE_VIRUS_RANGED : ETYPE_KPC;
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
    // Escudo de capsídeo no Mundo dos Vírus (reforçado para o mini chefe).
    if (virus) { m->shieldMaxHp = 120 + game->wave * 30; m->shieldHp = m->shieldMaxHp; m->shieldActive = true; }
    else       { m->shieldMaxHp = 0; m->shieldHp = 0; m->shieldActive = false; }
    m->shieldHitFlash = 0.0f;
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
        // Escolta temática por Mundo: bactérias (Mundo 1) ou vírus com escudo
        // (Mundo 2). Mantém um misto de corpo a corpo e atirador.
        bool virus = (game->currentWorld == WORLD_VIRUS);
        bool ranged = (GetRandomValue(0, 1) == 1);
        if (virus) e->type = ranged ? ETYPE_VIRUS_RANGED : ETYPE_VIRUS_MELEE;
        else       e->type = ranged ? ETYPE_BACT_RANGED  : ETYPE_BACT_MELEE;
        e->tier = TIER_2;
        e->isRanged = ranged;
        e->maxHp = 18 + game->wave * 5;
        e->hp = e->maxHp;
        e->speed = ranged ? 200.0f : 250.0f;
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
        // Escudo de capsídeo apenas no Mundo dos Vírus.
        if (virus) { e->shieldMaxHp = 25 + game->wave * 6; e->shieldHp = e->shieldMaxHp; e->shieldActive = true; }
        else       { e->shieldMaxHp = 0; e->shieldHp = 0; e->shieldActive = false; }
        e->shieldHitFlash = 0.0f;
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
            // Mantém o anel de escolta dentro do corpo.
            MapBody_ApplyCollision(&spawnPos, 30.0f);
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

        // -----------------------------------------------------------------
        // TIPO DO INIMIGO conforme o MUNDO atual (expansão em 2 Mundos):
        //  - WORLD_BACTERIA: só bactérias (melee/ranged) + elite KPC (onda >=3).
        //  - WORLD_VIRUS:    só vírus com escudo de capsídeo (melee/ranged).
        // Nenhum tipo legado (SARS/Dengue/Chagas) é spawnado aqui.
        // -----------------------------------------------------------------
        Enemy *e = &game->enemies[i];
        int randVal = GetRandomValue(0, 99);
        // Por padrão sem escudo; o Mundo dos Vírus ativa abaixo.
        e->shieldActive = false; e->shieldHp = 0; e->shieldMaxHp = 0; e->shieldHitFlash = 0.0f;

        if (game->currentWorld == WORLD_VIRUS)
        {
            bool ranged = (randVal < 45);
            if (ranged)
            {
                // Vírus à distância com escudo (tema influenza)
                e->type = ETYPE_VIRUS_RANGED;
                e->tier = TIER_2;
                e->isRanged = true;
                e->maxHp = 22 + game->wave * 6;
                e->speed = 200.0f + GetRandomValue(-15, 15);
            }
            else
            {
                // Vírus corpo a corpo com escudo (tema dengue)
                e->type = ETYPE_VIRUS_MELEE;
                e->tier = TIER_1;
                e->isRanged = false;
                e->maxHp = 30 + game->wave * 9;
                e->speed = 160.0f + GetRandomValue(-10, 10);
            }
            e->hp = e->maxHp;
            // Capsídeo: precisa ser quebrado antes de ferir a vida.
            e->shieldMaxHp = 28 + game->wave * 8;
            e->shieldHp = e->shieldMaxHp;
            e->shieldActive = true;
        }
        else // WORLD_BACTERIA
        {
            if (randVal < 50 || game->wave == 1)
            {
                // Bactéria corpo a corpo (cocos / pneumonia)
                e->type = ETYPE_BACT_MELEE;
                e->tier = TIER_1;
                e->isRanged = false;
                e->maxHp = 32 + game->wave * 10;
                e->speed = 150.0f + GetRandomValue(-10, 10);
            }
            else if (randVal < 85 || game->wave < 3)
            {
                // Bactéria à distância (bacilo atirador)
                e->type = ETYPE_BACT_RANGED;
                e->tier = TIER_2;
                e->isRanged = true;
                e->maxHp = 24 + game->wave * 6;
                e->speed = 110.0f + GetRandomValue(-10, 10);
            }
            else
            {
                // Elite KPC (tanque atirador) a partir da onda 3. O chefe em si
                // é criado por ConfigureBoss().
                e->type = ETYPE_KPC;
                e->tier = TIER_3;
                e->isRanged = true;
                e->maxHp = 200 + game->wave * 40;
                e->speed = 60.0f + GetRandomValue(-5, 5);
            }
            e->hp = e->maxHp;
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
                    MapBody_ApplyCollision(&ep, 30.0f); // dentro do corpo
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
        Vector2 itemPos = MapBody_RandomPointInside(game->player.position, 0.0f);
        SpawnPowerUpAt(game, itemPos, -1); // Tipo aleatório, dentro do corpo
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

    // Banner de aviso da nova horda / chefe (texto por Mundo).
    bool virusWorld = (game->currentWorld == WORLD_VIRUS);
    if (bossWave)
    {
        if (virusWorld)
            ShowBanner(game, "CHEFE VIRAL: CAPSIDEO REFORCADO",
                       "Quebre o escudo (capsideo) antes de atacar a vida!",
                       (Color){ 120, 200, 255, 255 }, 4.0f);
        else
            ShowBanner(game, "CHEFE: SUPERBACTERIA KPC",
                       "Resistente a antibioticos! Use a Granada e a BFG.",
                       (Color){ 255, 70, 90, 255 }, 4.0f);
        game->screenShake = 0.8f;
    }
    else
    {
        const char *sub;
        if (virusWorld)
            sub = (game->wave == 4)
                ? "Virus com escudo! Quebre o capsideo. O CHEFE viral se aproxima!"
                : "Virus com capsideo: quebre o escudo para eliminar.";
        else
            sub = (game->wave == 4)
                ? "MINI CHEFE presente! E a proxima horda traz o CHEFE final!"
                : "Um MINI CHEFE lidera esta horda. Elimine todos para avancar.";
        ShowBanner(game, TextFormat("HORDA %d / 5", game->wave), sub,
                   (Color){ 0, 229, 255, 255 }, 3.4f);
    }
}
