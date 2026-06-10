#include "../../include/gameplay.h"
#include "../../include/spatial_grid.h"
#include "../../Assets/Maps/map_seringa.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../include/asset_manager.h"
#include <string.h>
#include <time.h>
#include <process.h>
#include <math.h>
#include <time.h>
#include <string.h>

// ============================================================================
// AUXILIAR: EMISSOR DE PARTÍCULAS
// ============================================================================
void SpawnParticle(GameState *game, Vector2 position, Vector2 velocity, Color color, float size, float lifeTime)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (!game->particles[i].active)
        {
            game->particles[i].position = position;
            game->particles[i].velocity = velocity;
            game->particles[i].color = color;
            game->particles[i].size = size;
            game->particles[i].lifeTime = lifeTime;
            game->particles[i].maxLifeTime = lifeTime;
            game->particles[i].active = true;
            break;
        }
    }
}

// Emite uma explosão radial de partículas
static void SpawnParticleExplosion(GameState *game, Vector2 pos, Color col, int count, float minSpeed, float maxSpeed, float size, float life)
{
    for (int i = 0; i < count; i++)
    {
        float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
        float speed = (float)GetRandomValue((int)(minSpeed * 10), (int)(maxSpeed * 10)) / 10.0f;
        Vector2 vel = { cosf(angle) * speed, sinf(angle) * speed };
        SpawnParticle(game, pos, vel, col, size, life + (float)GetRandomValue(-2, 2)/10.0f);
    }
}

// ============================================================================
// AUXILIAR: SPAWN DE POWER-UPS
// ============================================================================
void SpawnPowerUpAt(GameState *game, Vector2 position, int forcedType)
{
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (!game->powerUps[i].active)
        {
            game->powerUps[i].position = position;
            game->powerUps[i].type = (forcedType >= 0) ? (PowerUpType)forcedType : (PowerUpType)GetRandomValue(0, 3);
            game->powerUps[i].active = true;
            game->powerUps[i].pulseTimer = 0.0f;
            break;
        }
    }
}

// ============================================================================
// AUXILIAR: SPAWN DE PROJÉTEIS
// ============================================================================
void SpawnProjectile(GameState *game, Vector2 pos, Vector2 target, ProjectileType type, int dmg)
{
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (!game->projectiles[i].active)
        {
            game->projectiles[i].position = pos;
            Vector2 dir = Vector2Normalize(Vector2Subtract(target, pos));
            float speed = 300.0f;
            if (type == PROJ_ACID_ARC) speed = 250.0f;
            if (type == PROJ_VOID_BOLT) speed = 400.0f;
            
            game->projectiles[i].velocity = Vector2Scale(dir, speed);
            game->projectiles[i].active = true;
            game->projectiles[i].type = type;
            game->projectiles[i].damage = dmg;
            game->projectiles[i].hitbox = (Rectangle){ pos.x - 10, pos.y - 10, 20, 20 };
            break;
        }
    }
}

// ============================================================================
// INICIALIZAÇÃO DO JOGO
// ============================================================================
void InitGame(GameState *game)
{
    // Preserva o nome do jogador se ja estiver definido
    char tempName[16] = "";
    float tempVol = 1.0f;
    if (game != NULL)
    {
        snprintf(tempName, sizeof(tempName), "%s", game->player.name);
        tempVol = game->masterVolume;
    }

    memset(game, 0, sizeof(GameState));

    if (tempName[0] != '\0')
    {
        snprintf(game->player.name, sizeof(game->player.name), "%s", tempName);
    }
    else
    {
        strcpy(game->player.name, "HERO");
    }
    game->masterVolume = tempVol;

    // Jogador inicial
    game->player.position = (Vector2){ MAP_WIDTH / 2.0f, MAP_HEIGHT / 2.0f };
    // Status base padrao
    game->player.speed = 280.0f;
    game->player.maxHp = 100;
    game->player.attackPower = 15;


    game->player.hp = game->player.maxHp;
    game->player.score = 0;
    game->player.level = 1;
    game->player.xp = 0;
    game->player.xpNeeded = 100;
    game->player.attackCooldown = 0.0f;

    // Sistema
    game->wave = 1;
    game->totalEnemiesKilled = 0;
    game->timeElapsed = 0.0f;
    game->screenShake = 0.0f;
    game->slashAnimTimer = 0.0f;

    // Câmera
    game->camera.target = game->player.position;
    game->camera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    game->camera.rotation = 0.0f;
    game->camera.zoom = 1.0f;

    // Limpa vetores
    for (int i = 0; i < MAX_ENEMIES; i++) game->enemies[i].active = false;
    for (int i = 0; i < MAX_POWERUPS; i++) game->powerUps[i].active = false;
    for (int i = 0; i < MAX_PARTICLES; i++) game->particles[i].active = false;

    // Inicia o Tutorial (Seringa de Vacina) em vez de ir diretamente à onda 1
    InitTutorial(game);
}

// ============================================================================
// PRÓXIMA ONDA DE INIMIGOS
// ============================================================================
void StartNextWave(GameState *game)
{
    // Aumenta quantidade a cada onda
    int numEnemies = 8 + game->wave * 4;
    if (numEnemies > MAX_ENEMIES) numEnemies = MAX_ENEMIES;

    game->enemiesRemaining = numEnemies;

    // Spawna inimigos longe do jogador (distância mínima de 450 px)
    for (int i = 0; i < numEnemies; i++)
    {
        Vector2 spawnPos;
        float distance = 0.0f;
        
        do
        {
            spawnPos.x = (float)GetRandomValue(100, MAP_WIDTH - 100);
            spawnPos.y = (float)GetRandomValue(100, MAP_HEIGHT - 100);
            distance = Vector2Distance(game->player.position, spawnPos);
        } while (distance < 450.0f);

        game->enemies[i].position = spawnPos;
        game->enemies[i].active = true;

        // Determina tipo e dificuldade do inimigo
        int randVal = GetRandomValue(0, 100);
        if (randVal < 60 || game->wave == 1)
        {
            // Inimigo Comum (Patrulha / Persegue)
            game->enemies[i].type = 0;
            game->enemies[i].tier = TIER_1;
            game->enemies[i].maxHp = 30 + game->wave * 10;
            game->enemies[i].hp = game->enemies[i].maxHp;
            game->enemies[i].speed = 140.0f + GetRandomValue(-20, 20);
        }
        else if (randVal < 80)
        {
            // Inimigo Rápido / Aedes
            game->enemies[i].type = 1;
            game->enemies[i].tier = TIER_2;
            game->enemies[i].isRanged = true;
            game->enemies[i].maxHp = 20 + game->wave * 5;
            game->enemies[i].hp = game->enemies[i].maxHp;
            game->enemies[i].speed = 210.0f + GetRandomValue(-15, 15);
        }
        else if (randVal < 90 && game->wave >= 3)
        {
            // Spawner / Foco de Dengue (Parado)
            game->enemies[i].type = 3;
            game->enemies[i].tier = TIER_2;
            game->enemies[i].maxHp = 150 + game->wave * 30;
            game->enemies[i].hp = game->enemies[i].maxHp;
            game->enemies[i].speed = 0.0f;
            game->enemies[i].isRanged = false;
            game->enemies[i].cooldownTimer = 5.0f; // Tempo entre spawns
        }
        else
        {
            // Inimigo Elite (Tamanho maior, muita vida)
            game->enemies[i].type = 2;
            if (game->wave >= 5 && i == 0) {
                game->enemies[i].tier = TIER_3_BOSS;
                game->enemies[i].maxHp = 1000;
            } else {
                game->enemies[i].tier = TIER_3;
                game->enemies[i].maxHp = 80 + game->wave * 25;
            }
            game->enemies[i].hp = game->enemies[i].maxHp;
            game->enemies[i].speed = 90.0f + GetRandomValue(-10, 10);
            game->enemies[i].isRanged = true;
        }

        game->enemies[i].state = IDLE;
        game->enemies[i].patrolTarget = spawnPos;
        game->enemies[i].patrolTimer = (float)GetRandomValue(2, 5);
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

// ============================================================================
// TUTORIAL: INICIALIZAÇÃO DA SERINGA DE VACINA
// ============================================================================
void InitTutorial(GameState *game)
{
    // Reseta flags do tutorial
    game->inTutorial           = true;
    game->tutorialStep         = 0;
    game->tutorialTimer        = 0.0f;
    game->tutorialEnemySpawned = false;

    // Inicia o diálogo do passo 0 (página 0)
    game->tutorialDialog.active    = true;
    game->tutorialDialog.page      = 0;
    game->tutorialDialog.charShown = 0;
    game->tutorialDialog.charTimer = 0.0f;

    // Reposiciona o jogador no centro da Seringa
    game->player.position = (Vector2){ SYRINGE_WIDTH / 2.0f, SYRINGE_HEIGHT / 2.0f };

    // Ajusta a câmera para o mapa menor da seringa
    game->camera.target = game->player.position;
    game->camera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    game->camera.zoom   = 1.0f;

    // Limpa power-ups anteriores e cria 3 novos para o Passo 0 (Treino de Movimentação)
    for (int i = 0; i < MAX_POWERUPS; i++) game->powerUps[i].active = false;
    
    // Esferas de treino (buffs)
    game->powerUps[0].position = (Vector2){ SYRINGE_WIDTH / 2.0f - 200.0f, SYRINGE_HEIGHT * 0.35f };
    game->powerUps[0].type = SPEED_BOOST;
    game->powerUps[0].active = true;
    game->powerUps[0].pulseTimer = 0.0f;

    game->powerUps[1].position = (Vector2){ SYRINGE_WIDTH / 2.0f + 200.0f, SYRINGE_HEIGHT * 0.45f };
    game->powerUps[1].type = ATTACK_BOOST;
    game->powerUps[1].active = true;
    game->powerUps[1].pulseTimer = 0.0f;

    game->powerUps[2].position = (Vector2){ SYRINGE_WIDTH / 2.0f, SYRINGE_HEIGHT * 0.58f };
    game->powerUps[2].type = SHIELD;
    game->powerUps[2].active = true;
    game->powerUps[2].pulseTimer = 0.0f;

    // Define a tela como Tutorial
    game->currentScreen = SCREEN_TUTORIAL;
}

// ============================================================================
// NÚmero de páginas de diálogo por passo do tutorial
// ============================================================================
static const int DIALOG_PAGES_PER_STEP[3] = { 2, 2, 2 };

// ============================================================================
// TUTORIAL: LÓGICA DE ATUALIZAÇÃO (3 PASSOS + SISTEMA DE DIÁLOGO)
// ============================================================================
void UpdateTutorial(GameState *game, float delta)
{
    UpdateGrid(game);
    DialogState *dlg = &game->tutorialDialog;

    // ========================================================================
    // SISTEMA DE DIÁLOGO: Typewriter + Q/ESPAÇO para avançar página
    // ========================================================================
    if (dlg->active)
    {
        // Obtém o texto do diálogo para o passo e página atuais para calcular o comprimento total
        const char *l1, *l2, *l3;
        GetTutorialDialogText(game->tutorialStep, dlg->page, &l1, &l2, &l3);
        int totalLen = strlen(l1) + strlen(l2) + strlen(l3);

        // --- Typewriter: revela um caractere a cada ~0.03s ---
        float typewriterSpeed = 0.030f; // segundos por caractere
        dlg->charTimer += delta;
        if (dlg->charTimer >= typewriterSpeed)
        {
            dlg->charTimer -= typewriterSpeed;
            if (dlg->charShown < totalLen)
            {
                dlg->charShown++;
            }
        }

        // --- Q ou ESPAÇO avançam o diálogo ---
        if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_SPACE))
        {
            if (dlg->charShown < totalLen)
            {
                // Se o texto ainda não terminou de aparecer: pula pro fim
                dlg->charShown = totalLen;
            }
            else
            {
                int maxPages = DIALOG_PAGES_PER_STEP[game->tutorialStep];

                // Verifica se ainda faltam páginas no passo atual
                if (dlg->page < maxPages - 1)
                {
                    // Próxima página
                    dlg->page++;
                    dlg->charShown = 0;
                    dlg->charTimer = 0.0f;
                }
                else
                {
                    // Última página do passo: fecha o diálogo e libera a gameplay
                    dlg->active    = false;
                    dlg->page      = 0;
                    dlg->charShown = 0;
                    dlg->charTimer = 0.0f;
                }
            }
        }
        // Diálogo ativo = só anima câmera, não move jogador nem processa passo
        game->camera.target.x += (game->player.position.x - game->camera.target.x) * 0.10f;
        game->camera.target.y += (game->player.position.y - game->camera.target.y) * 0.10f;
        return; // bloqueia resto do update durante o diálogo
    }

    // ========================================================================
    // MOVIMENTAÇÃO DO JOGADOR (só quando diálogo está fechado)
    // ========================================================================
    float currentSpeed = game->player.speed;
    if (game->player.speedTimer > 0.0f) currentSpeed *= 1.6f;

    Vector2 moveDir = { 0.0f, 0.0f };
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) { moveDir.x += 1.0f; game->player.facingDir = 1; }
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) { moveDir.x -= 1.0f; game->player.facingDir = -1; }
    if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) moveDir.y += 1.0f;
    if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) moveDir.y -= 1.0f;

    bool moving = (moveDir.x != 0.0f || moveDir.y != 0.0f);
    game->player.isMoving = moving;

    if (moving)
    {
        moveDir = Vector2Normalize(moveDir);
        game->player.position = Vector2Add(
            game->player.position,
            Vector2Scale(moveDir, currentSpeed * delta)
        );
        // Partículas de rastro suave
        if (GetRandomValue(0, 8) == 0)
        {
            Vector2 smokeVel = { -moveDir.x * 25.0f, -moveDir.y * 25.0f };
            SpawnParticle(game, game->player.position, smokeVel, Fade((Color){100,220,150,255}, 0.4f), 4.0f, 0.5f);
        }
    }


    // Colisão com paredes do mapa (via função do mapa da seringa)
    MapSeringa_ApplyWallCollision(&game->player.position, 20.0f);

    // --- Atualiza partículas ---
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (game->particles[i].active)
        {
            game->particles[i].velocity.y += 150.0f * delta;
            game->particles[i].velocity.x *= 0.96f; // Drag

            game->particles[i].position = Vector2Add(
                game->particles[i].position,
                Vector2Scale(game->particles[i].velocity, delta)
            );
            game->particles[i].lifeTime -= delta;
            if (game->particles[i].lifeTime <= 0.0f)
                game->particles[i].active = false;
        }
    }

    // Decaimento de cooldowns e buffs do jogador no tutorial
    if (game->player.attackCooldown > 0.0f) game->player.attackCooldown -= delta;
    if (game->player.speedTimer > 0.0f) game->player.speedTimer -= delta;
    if (game->player.shieldTimer > 0.0f) game->player.shieldTimer -= delta;
    if (game->player.attackBoostTimer > 0.0f) game->player.attackBoostTimer -= delta;
    if (game->slashAnimTimer > 0.0f) game->slashAnimTimer -= delta;

    // ========================================================================
    // PASSO 0: Coletar as 3 ampolas de vacina (Treino de Mobilidade)
    // ========================================================================
    if (game->tutorialStep == 0)
    {
        bool itemsRemaining = false;
        for (int i = 0; i < 3; i++)
        {
            if (game->powerUps[i].active)
            {
                itemsRemaining = true;
                break;
            }
        }

        if (!itemsRemaining)
        {
            game->tutorialStep = 1;
            game->tutorialTimer = 0.0f;
            // Abre diálogo do passo 1
            dlg->active    = true;
            dlg->page      = 0;
            dlg->charShown = 0;
            dlg->charTimer = 0.0f;
        }
    }

    // ========================================================================
    // PASSO 1: Combate — spawn de bactéria móvel e atiradora
    // ========================================================================
    else if (game->tutorialStep == 1)
    {
        // Spawna a bactéria uma única vez no passo 1
        if (!game->tutorialEnemySpawned)
        {
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (!game->enemies[i].active)
                {
                    game->enemies[i].active          = true;
                    game->enemies[i].position        = (Vector2){ SYRINGE_WIDTH / 2.0f + 250.0f, SYRINGE_HEIGHT / 2.0f - 100.0f };
                    game->enemies[i].hp              = 35;
                    game->enemies[i].maxHp           = 35;
                    game->enemies[i].speed           = 85.0f;
                    game->enemies[i].type            = 0;
                    game->enemies[i].tier            = TIER_1;
                    game->enemies[i].state           = IDLE;
                    game->enemies[i].isRanged        = false; // controlada de forma customizada abaixo
                    game->enemies[i].isTutorialEnemy = true;
                    game->enemies[i].cooldownTimer   = 1.5f; // primeiro tiro após 1.5s
                    game->enemies[i].patrolTarget    = game->enemies[i].position;
                    game->enemies[i].patrolTimer     = 99.0f;
                    break;
                }
            }
            game->tutorialEnemySpawned = true;
            // Partículas de invocação
            for (int p = 0; p < 15; p++)
            {
                Vector2 vel = { (float)GetRandomValue(-80, 80), (float)GetRandomValue(-80, 80) };
                SpawnParticle(game, (Vector2){ SYRINGE_WIDTH/2.0f+250, SYRINGE_HEIGHT/2.0f-100 }, vel, (Color){50,200,80,255}, 5.0f, 0.8f);
            }
        }

        // IA da bactéria tutorial e seu status de morte
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (game->enemies[i].active && game->enemies[i].isTutorialEnemy)
            {
                Enemy *e = &game->enemies[i];
                if (e->state == DEATH)
                {
                    e->cooldownTimer -= delta;
                    if (e->cooldownTimer <= 0.0f)
                    {
                        e->active = false;
                        // Spawna a célula de cura ao morrer!
                        SpawnPowerUpAt(game, e->position, HP_RECOVERY);
                    }
                    continue;
                }

                if (e->state == HURT)
                {
                    e->cooldownTimer -= delta;
                    if (e->cooldownTimer <= 0.0f) e->state = IDLE;
                }
                else if (e->state == ATTACK)
                {
                    e->chargeTimer -= delta;
                    if (e->chargeTimer <= 0.0f)
                    {
                        // Dispara uma toxina lenta de treino em direção ao jogador
                        Vector2 targetDir = Vector2Subtract(game->player.position, e->position);
                        targetDir = Vector2Normalize(targetDir);
                        Vector2 projVel = Vector2Scale(targetDir, 160.0f); // Tiro lento de 160 px/s

                        // Procura slot de projétil
                        for (int j = 0; j < MAX_PROJECTILES; j++)
                        {
                            if (!game->projectiles[j].active)
                            {
                                game->projectiles[j].active = true;
                                game->projectiles[j].position = e->position;
                                game->projectiles[j].velocity = projVel;
                                game->projectiles[j].type = PROJ_ACID_ARC;
                                game->projectiles[j].damage = 6;
                                game->projectiles[j].hitbox = (Rectangle){ e->position.x - 10, e->position.y - 10, 20, 20 };
                                break;
                            }
                        }
                        e->state = IDLE;
                    }
                }
                else
                {
                    // Persegue o jogador lentamente
                    Vector2 chaseDir = Vector2Subtract(game->player.position, e->position);
                    float dist = Vector2Length(chaseDir);
                    if (dist > 15.0f)
                    {
                        chaseDir = Vector2Normalize(chaseDir);
                        e->position = Vector2Add(e->position, Vector2Scale(chaseDir, e->speed * delta));
                        e->state = AGGRO;
                    }
                    else
                    {
                        e->state = IDLE;
                    }

                    // Ataca atirando a cada 3.2s se perto
                    e->cooldownTimer -= delta;
                    if (e->cooldownTimer <= 0.0f && dist < 400.0f)
                    {
                        e->state = ATTACK;
                        e->chargeTimer = 0.5f;     // Tempo de carregamento do tiro
                        e->cooldownTimer = 3.2f;   // Cooldown entre tiros
                    }
                }
            }
        }

        // Ataque com ESPAÇO ou clique (Q está reservado para diálogo)
        if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            PlayerAttack(game);

        // Verifica se a bactéria tutorial foi eliminada
        bool algumVivo = false;
        for (int i = 0; i < MAX_ENEMIES; i++)
            if (game->enemies[i].active && game->enemies[i].isTutorialEnemy)
            { algumVivo = true; break; }

        if (!algumVivo && game->tutorialEnemySpawned)
        {
            game->tutorialStep  = 2;
            game->tutorialTimer = 0.0f;
            // Abre diálogo do passo 2
            dlg->active    = true;
            dlg->page      = 0;
            dlg->charShown = 0;
            dlg->charTimer = 0.0f;
        }
    }

    // ========================================================================
    // PASSO 2: Saída pela agulha (bocal da seringa)
    // ========================================================================
    else if (game->tutorialStep == 2)
    {
        Rectangle playerRect = {
            game->player.position.x - 18.0f,
            game->player.position.y - 18.0f,
            36.0f, 36.0f
        };

        if (MapSeringa_CheckExit(playerRect))
        {
            game->inTutorial = false;

            // Limpa inimigos e partículas da seringa
            for (int i = 0; i < MAX_ENEMIES;  i++) game->enemies[i].active   = false;
            for (int i = 0; i < MAX_PARTICLES; i++) game->particles[i].active = false;

            // Tela de carregamento para o organismo
            RequestLoadingScreen(game, LOAD_TO_GAMEPLAY, 2.0f);
        }
    }

    // ========================================================================
    // ATUALIZA COLISÕES E PROJÉTEIS DENTRO DO TUTORIAL
    // ========================================================================
    // 1. Colisão Jogador <-> Bactéria
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (game->enemies[i].active && game->enemies[i].isTutorialEnemy && game->enemies[i].state != DEATH)
        {
            float dist = Vector2Distance(game->player.position, game->enemies[i].position);
            if (dist < 35.0f) // colidiu
            {
                if (game->player.shieldTimer > 0.0f)
                {
                    SpawnParticleExplosion(game, game->player.position, SKYBLUE, 10, 80.0f, 150.0f, 3.0f, 0.4f);
                }
                else
                {
                    game->player.hp -= 5;
                    game->screenShake = 0.35f;
                    SpawnParticleExplosion(game, game->player.position, RED, 12, 50.0f, 120.0f, 3.5f, 0.5f);
                    
                    if (game->player.hp <= 0)
                    {
                        game->player.hp = 0;
                        game->currentScreen = SCREEN_GAMEOVER;
                        return;
                    }
                }
                
                // Repele o inimigo ligeiramente para trás
                Vector2 pushDir = Vector2Subtract(game->enemies[i].position, game->player.position);
                if (pushDir.x == 0.0f && pushDir.y == 0.0f) pushDir = (Vector2){ 0.0f, 1.0f };
                pushDir = Vector2Normalize(pushDir);
                game->enemies[i].position = Vector2Add(game->enemies[i].position, Vector2Scale(pushDir, 60.0f));
                game->enemies[i].state = HURT;
                game->enemies[i].cooldownTimer = 0.5f; // stun
            }
        }
    }

    // 2. Colisão Projétil <-> Jogador
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (game->projectiles[i].active)
        {
            game->projectiles[i].position = Vector2Add(
                game->projectiles[i].position,
                Vector2Scale(game->projectiles[i].velocity, delta)
            );
            game->projectiles[i].hitbox.x = game->projectiles[i].position.x - 10;
            game->projectiles[i].hitbox.y = game->projectiles[i].position.y - 10;

            Rectangle pRect = { game->player.position.x - 20, game->player.position.y - 20, 40, 40 };
            if (CheckCollisionRecs(game->projectiles[i].hitbox, pRect))
            {
                game->projectiles[i].active = false;
                if (game->player.shieldTimer > 0.0f)
                {
                    SpawnParticleExplosion(game, game->player.position, SKYBLUE, 10, 80.0f, 150.0f, 3.0f, 0.4f);
                }
                else
                {
                    game->player.hp -= game->projectiles[i].damage;
                    game->screenShake = 0.3f;
                    SpawnParticleExplosion(game, game->player.position, RED, 10, 50.0f, 100.0f, 3.0f, 0.5f);
                    if (game->player.hp <= 0)
                    {
                        game->player.hp = 0;
                        game->currentScreen = SCREEN_GAMEOVER;
                        return;
                    }
                }
            }

            if (game->projectiles[i].position.x < -100 || game->projectiles[i].position.x > SYRINGE_WIDTH + 100 ||
                game->projectiles[i].position.y < -100 || game->projectiles[i].position.y > SYRINGE_HEIGHT + 100)
            {
                game->projectiles[i].active = false;
            }
        }
    }

    // 3. Coleta de Power-ups (ampolas de vacina / esferas de treino e cura)
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (game->powerUps[i].active)
        {
            game->powerUps[i].pulseTimer += delta;
            float dist = Vector2Distance(game->player.position, game->powerUps[i].position);
            if (dist < 35.0f) // coletou
            {
                game->powerUps[i].active = false;
                game->player.score += 50;
                PlaySound(g_assets.sfxPickup);

                SpawnParticleExplosion(game, game->powerUps[i].position, YELLOW, 15, 60.0f, 160.0f, 4.5f, 0.6f);

                switch (game->powerUps[i].type)
                {
                    case HP_RECOVERY:
                        game->player.hp += 35;
                        if (game->player.hp > game->player.maxHp) game->player.hp = game->player.maxHp;
                        SpawnParticleExplosion(game, game->player.position, GREEN, 15, 40.0f, 100.0f, 4.0f, 0.8f);
                        break;
                    case SPEED_BOOST:
                        game->player.speedTimer = 8.0f;
                        break;
                    case SHIELD:
                        game->player.shieldTimer = 7.0f;
                        break;
                    case ATTACK_BOOST:
                        game->player.attackBoostTimer = 8.0f;
                        break;
                }
            }
        }
    }

    // Atualiza câmera suavemente
    game->camera.target.x += (game->player.position.x - game->camera.target.x) * 0.10f;
    game->camera.target.y += (game->player.position.y - game->camera.target.y) * 0.10f;
}

// ============================================================================
// LOGICA DE COMBATE: ATAQUE DO JOGADOR
// ============================================================================
void PlayerAttack(GameState *game)
{
    if (game->player.attackCooldown > 0.0f) return;

    // Define cooldown base
    game->player.attackCooldown = 0.35f;

    PlaySound(g_assets.sfxAttack);

    // Configura animação de ataque (Slash)
    game->slashAnimTimer = 0.22f;
    game->slashAnimPos = game->player.position;
    game->slashAnimRadius = 140.0f;

    // Determina força do ataque
    int dano = game->player.attackPower;
    if (game->player.attackBoostTimer > 0.0f) dano *= 2; // Buff de ataque dobra dano

    // Efeitos visuais: explosão de partículas de slash (brancas e azul claro)
    SpawnParticleExplosion(game, game->player.position, LIGHTGRAY, 12, 100.0f, 250.0f, 4.0f, 0.4f);
    SpawnParticleExplosion(game, game->player.position, SKYBLUE, 8, 150.0f, 300.0f, 3.5f, 0.35f);

    // Câmera dá uma leve chacoalhada no ataque
    game->screenShake = 0.25f;

    // Verifica colisão com inimigos na área do ataque usando Spatial Grid
    int collIndices[MAX_ENEMIES];
    int collCount = GetEnemiesInRadius(game, game->player.position, game->slashAnimRadius, collIndices);

    for (int k = 0; k < collCount; k++)
    {
        int i = collIndices[k];
        if (!game->enemies[i].active || game->enemies[i].state == DEATH) continue;

        // Acertou! Causa dano
            game->enemies[i].hp -= dano;
            PlaySound(g_assets.sfxEnemyHurt);

            // Empurrão (Knockback) na direção oposta ao jogador
            Vector2 knockbackDir = Vector2Subtract(game->enemies[i].position, game->player.position);
            if (knockbackDir.x == 0.0f && knockbackDir.y == 0.0f) knockbackDir = (Vector2){ 1.0f, 0.0f };
            knockbackDir = Vector2Normalize(knockbackDir);
            
            // Empurra o inimigo a uma distância segura
            game->enemies[i].position = Vector2Add(game->enemies[i].position, Vector2Scale(knockbackDir, 55.0f));

            // Partículas de sangue/dano no local do inimigo
            Color hitColor = (game->enemies[i].type == 2) ? MAROON : RED;
            SpawnParticleExplosion(game, game->enemies[i].position, hitColor, 15, 80.0f, 180.0f, 3.0f, 0.5f);

            // Se o inimigo morreu
            if (game->enemies[i].hp <= 0)
            {
                game->enemies[i].state = DEATH;
                game->enemies[i].cooldownTimer = 0.5f; // Duração da animação de morte

                // Partículas de morte e ganho de xp
                SpawnParticleExplosion(game, game->enemies[i].position, GOLD, 20, 50.0f, 150.0f, 4.0f, 0.7f);

                // Durante o tutorial NÃO contabilizamos onda nem score real
                // — o progresso é controlado em UpdateTutorial()
                if (game->inTutorial)
                {
                    // Apenas efeito visual de vitória na seringa
                    SpawnParticleExplosion(game, game->enemies[i].position, (Color){0, 220, 120, 255}, 15, 60.0f, 140.0f, 4.0f, 0.7f);
                    continue;
                }

                // --- Lógica normal de jogo (fora do tutorial) ---
                game->enemiesRemaining--;
                game->totalEnemiesKilled++;

                // Aumenta score
                int xpGanho = 20 * (game->enemies[i].type + 1);
                int scoreGanho = 100 * (game->enemies[i].type + 1);
                game->player.score += scoreGanho;
                game->player.xp += xpGanho;

                // Chance de drop de PowerUp (25%)
                if (GetRandomValue(0, 100) < 25)
                {
                    SpawnPowerUpAt(game, game->enemies[i].position, -1);
                }

                // Verifica se eliminou todos os inimigos da onda
                if (game->enemiesRemaining <= 0)
                {
                    game->wave++;
                    if (game->wave > 5)
                    {
                        RequestLoadingScreen(game, LOAD_TO_VICTORY, 2.5f);
                    }
                    else
                    {
                        game->currentScreen = SCREEN_QUIZ;
                    }
                    return;
                }
            }
            else
            {
                // Se o inimigo não morreu, entra em estado de ferimento (stun/flash)
                game->enemies[i].state = HURT;
                game->enemies[i].cooldownTimer = 0.25f; // flash/stun de 0.25s

        }
    }
}

// ============================================================================
// CICLO PRINCIPAL DE ATUALIZAÇÃO DA LOGICA
// ============================================================================
void UpdateGameplay(GameState *game, float delta)
{
    UpdateGrid(game);
    game->timeElapsed += delta;

    // Decaimento do screen shake
    if (game->screenShake > 0.0f)
    {
        game->screenShake -= delta * 1.5f;
        if (game->screenShake < 0.0f) game->screenShake = 0.0f;
    }

    // Decaimento da animação de slash
    if (game->slashAnimTimer > 0.0f)
    {
        game->slashAnimTimer -= delta;
    }

    // ------------------------------------------------------------------------
    // 1. ATUALIZA TIMERS E STATS DO JOGADOR
    // ------------------------------------------------------------------------
    if (game->player.speedTimer > 0.0f) game->player.speedTimer -= delta;
    if (game->player.shieldTimer > 0.0f) game->player.shieldTimer -= delta;
    if (game->player.attackBoostTimer > 0.0f) game->player.attackBoostTimer -= delta;
    if (game->player.attackCooldown > 0.0f) game->player.attackCooldown -= delta;
    if (game->player.poisonTimer > 0.0f) {
        game->player.poisonTimer -= delta;
        // Dano de poison a cada frame (equivalente a 8 dano por segundo)
        game->player.hp -= 8 * delta;
        if (game->player.hp <= 0) {
            game->player.hp = 0;
            game->currentScreen = SCREEN_GAMEOVER;
            return;
        }
        if (GetRandomValue(0, 30) == 0) {
            SpawnParticle(game, game->player.position, (Vector2){0, -20}, PURPLE, 4.0f, 0.5f);
        }
    }
    if (game->player.slowTimer > 0.0f) game->player.slowTimer -= delta;

    // ------------------------------------------------------------------------
    // 2. MOVIMENTAÇÃO DO JOGADOR
    // ------------------------------------------------------------------------
    float currentSpeed = game->player.speed;
    if (game->player.speedTimer > 0.0f) currentSpeed *= 1.6f; // Buff de velocidade
    if (game->player.slowTimer > 0.0f) currentSpeed *= 0.5f;  // Debuff de velocidade

    Vector2 moveDir = { 0.0f, 0.0f };
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) { moveDir.x += 1.0f; game->player.facingDir = 1; }
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  { moveDir.x -= 1.0f; game->player.facingDir = -1; }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))  moveDir.y += 1.0f;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))    moveDir.y -= 1.0f;

    if (moveDir.x != 0.0f || moveDir.y != 0.0f)
    {
        game->player.isMoving = true;
        moveDir = Vector2Normalize(moveDir);
        game->player.position = Vector2Add(
            game->player.position,
            Vector2Scale(moveDir, currentSpeed * delta)
        );

        // Emite fumaça suave de movimento
        if (GetRandomValue(0, 10) == 0)
        {
            Vector2 smokeVel = { -moveDir.x * 30.0f, -moveDir.y * 30.0f };
            Color pCol = (game->player.speedTimer > 0.0f) ? YELLOW : LIGHTGRAY;
            SpawnParticle(game, game->player.position, smokeVel, Fade(pCol, 0.4f), 5.0f, 0.5f);
        }
    }
    else
    {
        game->player.isMoving = false;
    }

    // Limites do mapa para o jogador
    float playerRadius = 20.0f;
    if (game->player.position.x < playerRadius) game->player.position.x = playerRadius;
    if (game->player.position.x > MAP_WIDTH - playerRadius) game->player.position.x = MAP_WIDTH - playerRadius;
    if (game->player.position.y < playerRadius) game->player.position.y = playerRadius;
    if (game->player.position.y > MAP_HEIGHT - playerRadius) game->player.position.y = MAP_HEIGHT - playerRadius;

    // ------------------------------------------------------------------------
    // 3. ENTRADA DE COMBATE
    // ------------------------------------------------------------------------
    if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        PlayerAttack(game);
    }

    // Entrada de combate e fluxo de save serao tratados no rpg.c para capturar a screenshot

    // ------------------------------------------------------------------------
    // 4. ATUALIZA PARTÍCULAS
    // ------------------------------------------------------------------------
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (game->particles[i].active)
        {
            // Partículas com gravidade direcional e arraste (viscosidade)
            game->particles[i].velocity.y += 150.0f * delta;
            game->particles[i].velocity.x *= 0.96f; // Drag
            
            game->particles[i].position = Vector2Add(
                game->particles[i].position,
                Vector2Scale(game->particles[i].velocity, delta)
            );
            game->particles[i].lifeTime -= delta;
            if (game->particles[i].lifeTime <= 0.0f)
            {
                game->particles[i].active = false;
            }
        }
    }

    // ------------------------------------------------------------------------
    // 5. ATUALIZA POWER-UPS (COLETA)
    // ------------------------------------------------------------------------
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (game->powerUps[i].active)
        {
            game->powerUps[i].pulseTimer += delta;

            // Distância jogador -> item
            float dist = Vector2Distance(game->player.position, game->powerUps[i].position);
            if (dist < 35.0f) // Colidiu/Coletou
            {
                game->powerUps[i].active = false;
                game->player.score += 50;
                PlaySound(g_assets.sfxPickup);

                // Efeito radial de coleta
                SpawnParticleExplosion(game, game->powerUps[i].position, YELLOW, 15, 60.0f, 160.0f, 4.5f, 0.6f);

                // Aplica efeitos baseados no tipo do Power-up
                switch (game->powerUps[i].type)
                {
                    case HP_RECOVERY:
                        game->player.hp += 35;
                        if (game->player.hp > game->player.maxHp) game->player.hp = game->player.maxHp;
                        // Partículas verdes para cura
                        SpawnParticleExplosion(game, game->player.position, GREEN, 15, 40.0f, 100.0f, 4.0f, 0.8f);
                        break;
                    case SPEED_BOOST:
                        game->player.speedTimer = 8.0f; // 8 segundos
                        break;
                    case SHIELD:
                        game->player.shieldTimer = 7.0f; // 7 segundos
                        break;
                    case ATTACK_BOOST:
                        game->player.attackBoostTimer = 8.0f; // 8 segundos
                        break;
                }
            }
        }
    }

    // ------------------------------------------------------------------------
    // 6. ATUALIZA INIMIGOS E INTELIGÊNCIA ARTIFICIAL (IA)
    // ------------------------------------------------------------------------
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!game->enemies[i].active) continue;

        Enemy *enemy = &game->enemies[i];

        if (enemy->state == DEATH)
        {
            enemy->cooldownTimer -= delta;
            if (enemy->cooldownTimer <= 0.0f)
            {
                enemy->active = false;
            }
            continue;
        }
        
        float distToPlayer = Vector2Distance(game->player.position, enemy->position);

        // State Machine Update
        if (enemy->state == HURT) {
            enemy->cooldownTimer -= delta;
            if (enemy->cooldownTimer <= 0.0f) enemy->state = IDLE;
        }
        else if (enemy->state == ATTACK) {
            enemy->chargeTimer -= delta;
            if (enemy->chargeTimer <= 0.0f) {
                // Atira
                ProjectileType ptype = PROJ_ACID_ARC;
                if (enemy->tier == TIER_2) ptype = PROJ_BULLET_SPREAD;
                if (enemy->tier >= TIER_3) ptype = PROJ_VOID_BOLT;
                
                int dmg = 10 + enemy->tier * 5;
                SpawnProjectile(game, enemy->position, game->player.position, ptype, dmg);
                PlaySound(g_assets.sfxEnemyShoot);
                if (enemy->tier == TIER_3_BOSS) {
                    Vector2 off1 = { game->player.position.x + 100, game->player.position.y };
                    Vector2 off2 = { game->player.position.x - 100, game->player.position.y };
                    SpawnProjectile(game, enemy->position, off1, ptype, dmg);
                    SpawnProjectile(game, enemy->position, off2, ptype, dmg);
                }
                
                enemy->state = IDLE;
                enemy->cooldownTimer = 1.2f; // cooldown
            }
        }
        else if (enemy->isRanged && distToPlayer < 400.0f && enemy->cooldownTimer <= 0.0f) {
            enemy->state = ATTACK;
            enemy->chargeTimer = 0.6f;
        }
        else if (distToPlayer < 450.0f) {
            enemy->state = AGGRO;
        }
        else {
            enemy->state = IDLE;
            enemy->patrolTimer -= delta;
        }
        
        if (enemy->cooldownTimer > 0.0f && enemy->state != HURT && enemy->state != ATTACK) {
            enemy->cooldownTimer -= delta;
        }

        // Lógica do Spawner (Type 3)
        if (enemy->type == 3 && enemy->cooldownTimer <= 0.0f && enemy->state != DEATH) {
            // Tenta spawnar um Aedes (type 1)
            for (int k = 0; k < MAX_ENEMIES; k++) {
                if (!game->enemies[k].active) {
                    game->enemies[k].active = true;
                    game->enemies[k].position = enemy->position;
                    game->enemies[k].type = 1; // Aedes
                    game->enemies[k].tier = TIER_2;
                    game->enemies[k].maxHp = 20 + game->wave * 5;
                    game->enemies[k].hp = game->enemies[k].maxHp;
                    game->enemies[k].speed = 210.0f + GetRandomValue(-15, 15);
                    game->enemies[k].isRanged = true;
                    game->enemies[k].state = IDLE;
                    game->enemies[k].cooldownTimer = 2.0f;
                    game->enemiesRemaining++; // Conta para a onda
                    // Partícula de spawn
                    SpawnParticleExplosion(game, enemy->position, DARKGRAY, 10, 50.0f, 150.0f, 2.0f, 0.4f);
                    break;
                }
            }
            enemy->cooldownTimer = 6.0f; // Tempo para o próximo spawn
        }

        // Ações de Movimentação
        if (enemy->state == AGGRO)
        {
            Vector2 chaseDir = Vector2Subtract(game->player.position, enemy->position);
            float distToPlayer = Vector2Length(chaseDir);
            chaseDir = Vector2Normalize(chaseDir);

            if (!enemy->isRanged) {
                // Inimigo melee comum (Patrulha / Persegue)
                float chaseMult = (enemy->tier == TIER_2) ? 1.25f : 1.05f;
                enemy->position = Vector2Add(enemy->position, Vector2Scale(chaseDir, enemy->speed * chaseMult * delta));
            } else {
                // IA Específica: Aedes aegypti (Type 1) e KPC (Type 2)
                if (enemy->type == 1) { // Aedes aegypti (errático, tenta manter distância)
                    if (distToPlayer > 300.0f) {
                        float timeFactor = GetTime() * 15.0f;
                        float angle = sinf(timeFactor) * 1.2f;
                        float nx = chaseDir.x * cosf(angle) - chaseDir.y * sinf(angle);
                        float ny = chaseDir.x * sinf(angle) + chaseDir.y * cosf(angle);
                        Vector2 zigzagDir = (Vector2){nx, ny};
                        enemy->position = Vector2Add(enemy->position, Vector2Scale(zigzagDir, enemy->speed * delta));
                    } else if (distToPlayer < 200.0f) {
                        // Foge se o jogador chegar muito perto
                        enemy->position = Vector2Add(enemy->position, Vector2Scale(chaseDir, -enemy->speed * delta));
                    }
                } else if (enemy->type == 2) { // KPC / Superbactéria (Lento, mas persegue constantemente)
                    if (distToPlayer > 250.0f) {
                        enemy->position = Vector2Add(enemy->position, Vector2Scale(chaseDir, enemy->speed * 0.8f * delta));
                    }
                }
            }
        }
        else if (enemy->state == IDLE)
        {
            float distToTarget = Vector2Distance(enemy->position, enemy->patrolTarget);
            if (distToTarget < 15.0f || enemy->patrolTimer <= 0.0f)
            {
                float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
                float radius = (float)GetRandomValue(100, 300);
                enemy->patrolTarget.x = enemy->position.x + cosf(angle) * radius;
                enemy->patrolTarget.y = enemy->position.y + sinf(angle) * radius;
                enemy->patrolTimer = (float)GetRandomValue(3, 7);
            }
            Vector2 patrolDir = Vector2Normalize(Vector2Subtract(enemy->patrolTarget, enemy->position));
            enemy->position = Vector2Add(enemy->position, Vector2Scale(patrolDir, enemy->speed * delta));
        }

        // --------------------------------------------------------------------
        // FÍSICA DE COLISÃO DESLIZANTE (SEPARAÇÃO ENTRE INIMIGOS)
        // --------------------------------------------------------------------
        float enemyRadius = (enemy->type == 2) ? 35.0f : 20.0f;
        for (int j = 0; j < MAX_ENEMIES; j++) {
            if (i != j && game->enemies[j].active && game->enemies[j].state != DEATH) {
                float otherRadius = (game->enemies[j].type == 2) ? 35.0f : 20.0f;
                float dist = Vector2Distance(enemy->position, game->enemies[j].position);
                float minDist = enemyRadius + otherRadius;
                if (dist < minDist && dist > 0.01f) {
                    Vector2 separationDir = Vector2Subtract(enemy->position, game->enemies[j].position);
                    separationDir = Vector2Normalize(separationDir);
                    // Empurrão de separação ajustável
                    enemy->position = Vector2Add(enemy->position, Vector2Scale(separationDir, 60.0f * delta));
                }
            }
        }

        // Mantém inimigos nos limites do mapa
        if (enemy->position.x < enemyRadius) enemy->position.x = enemyRadius;
        if (enemy->position.x > MAP_WIDTH - enemyRadius) enemy->position.x = MAP_WIDTH - enemyRadius;
        if (enemy->position.y < enemyRadius) enemy->position.y = enemyRadius;
        if (enemy->position.y > MAP_HEIGHT - enemyRadius) enemy->position.y = MAP_HEIGHT - enemyRadius;

        // --------------------------------------------------------------------
        // COLISÃO: DANO NO JOGADOR
        // --------------------------------------------------------------------
        float colRange = (enemy->type == 2) ? 45.0f : 30.0f;
        if (distToPlayer < colRange)
        {
            // Se o escudo estiver ativo, absorve o dano e cria faíscas azuis
            if (game->player.shieldTimer > 0.0f)
            {
                SpawnParticleExplosion(game, game->player.position, SKYBLUE, 10, 80.0f, 150.0f, 3.0f, 0.4f);
            }
            else
            {
                // Causa dano
                int dmgBase = 8;
                if (enemy->type == 1) { 
                    dmgBase = 12; // Aedes
                    game->player.poisonTimer = 3.0f; // Aedes transmite poison na picada
                }
                if (enemy->type == 2) { 
                    dmgBase = 22; // Elite/KPC
                    game->player.slowTimer = 2.0f; // KPC deixa o jogador lento
                }
                
                game->player.hp -= dmgBase;
                game->screenShake = 0.4f;

                // Partículas vermelhas de ferimento
                SpawnParticleExplosion(game, game->player.position, RED, 12, 50.0f, 130.0f, 3.5f, 0.5f);

                // Checa Game Over
                if (game->player.hp <= 0)
                {
                    PlaySound(g_assets.sfxDeath);
                    game->player.hp = 0;
                    game->currentScreen = SCREEN_GAMEOVER;
                    return;
                } else {
                    PlaySound(g_assets.sfxHurt);
                }
            }

            // Repele o inimigo ligeiramente para trás após causar o dano
            Vector2 pushDir = Vector2Subtract(enemy->position, game->player.position);
            if (pushDir.x == 0.0f && pushDir.y == 0.0f) pushDir = (Vector2){ 0.0f, 1.0f };
            pushDir = Vector2Normalize(pushDir);
            enemy->position = Vector2Add(enemy->position, Vector2Scale(pushDir, 50.0f));
            enemy->state = HURT;
            enemy->cooldownTimer = 0.5f; // stun duration
        }
    }

    // ------------------------------------------------------------------------
    // 6.5 ATUALIZA PROJÉTEIS
    // ------------------------------------------------------------------------
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (game->projectiles[i].active)
        {
            game->projectiles[i].position = Vector2Add(game->projectiles[i].position, Vector2Scale(game->projectiles[i].velocity, delta));
            game->projectiles[i].hitbox.x = game->projectiles[i].position.x - 10;
            game->projectiles[i].hitbox.y = game->projectiles[i].position.y - 10;
            
            // Colisão com jogador
            Rectangle pRect = { game->player.position.x - 20, game->player.position.y - 20, 40, 40 };
            if (CheckCollisionRecs(game->projectiles[i].hitbox, pRect))
            {
                game->projectiles[i].active = false;
                if (game->player.shieldTimer > 0.0f) {
                    SpawnParticleExplosion(game, game->player.position, SKYBLUE, 10, 80.0f, 150.0f, 3.0f, 0.4f);
                } else {
                    game->player.hp -= game->projectiles[i].damage;
                    
                    // Aplica Debuffs baseado no tipo de projétil
                    if (game->projectiles[i].type == PROJ_ACID_ARC) {
                        game->player.poisonTimer = 3.0f; // 3 seg de poison
                    } else if (game->projectiles[i].type == PROJ_VOID_BOLT) {
                        game->player.slowTimer = 2.0f; // 2 seg de slow
                    }

                    game->screenShake = 0.3f;
                    SpawnParticleExplosion(game, game->player.position, RED, 10, 50.0f, 100.0f, 3.0f, 0.5f);
                    if (game->player.hp <= 0) {
                        PlaySound(g_assets.sfxDeath);
                        game->player.hp = 0;
                        game->currentScreen = SCREEN_GAMEOVER;
                        return;
                    } else {
                        PlaySound(g_assets.sfxHurt);
                    }
                }
            }
            
            // Remove se sair do mapa longe
            if (game->projectiles[i].position.x < -100 || game->projectiles[i].position.x > MAP_WIDTH + 100 ||
                game->projectiles[i].position.y < -100 || game->projectiles[i].position.y > MAP_HEIGHT + 100) {
                game->projectiles[i].active = false;
            }
        }
    }

    // ------------------------------------------------------------------------
    // 7. SISTEMA DE LEVEL UP DO HERÓI
    // ------------------------------------------------------------------------
    if (game->player.xp >= game->player.xpNeeded)
    {
        // Consome XP e sobe de nível
        game->player.xp -= game->player.xpNeeded;
        game->player.level++;
        game->player.xpNeeded = (int)(game->player.xpNeeded * 1.5f);

        // Melhora atributos do herói
        game->player.maxHp += 15;
        game->player.hp = game->player.maxHp; // Cura total no level up
        game->player.attackPower += 6;
        game->player.speed += 10.0f;

        // Grande efeito visual festivo de Level Up (Verde e Gold)
        SpawnParticleExplosion(game, game->player.position, LIME, 30, 80.0f, 220.0f, 5.0f, 1.0f);
        SpawnParticleExplosion(game, game->player.position, GOLD, 20, 100.0f, 250.0f, 4.0f, 1.2f);
        
        game->screenShake = 0.5f;
    }

    // ------------------------------------------------------------------------
    // 8. ATUALIZAÇÃO DA CÂMERA (SUAVE COM LERP + SHAKE)
    // ------------------------------------------------------------------------
    float targetX = game->player.position.x;
    float targetY = game->player.position.y;
    
    // Lerp da câmera em direção ao jogador
    game->camera.target.x += (targetX - game->camera.target.x) * 0.085f;
    game->camera.target.y += (targetY - game->camera.target.y) * 0.085f;

    // Aplica chacoalhar de tela na câmera se ativo
    if (game->screenShake > 0.0f)
    {
        game->camera.offset.x = (SCREEN_WIDTH / 2.0f) + (float)GetRandomValue(-15, 15) * game->screenShake;
        game->camera.offset.y = (SCREEN_HEIGHT / 2.0f) + (float)GetRandomValue(-15, 15) * game->screenShake;
    }
    else
    {
        game->camera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    }
}

// ============================================================================
// PERSISTÊNCIA: SALVAR JOGO POR SLOT
// ============================================================================
typedef struct {
    GameState *gameSnapshot;
    int slot;
} SaveThreadData;

void SaveGameThread(void *lpParam)
{
    SaveThreadData *data = (SaveThreadData *)lpParam;
    GameState *game = data->gameSnapshot;
    int slot = data->slot;

    char path[64];
    sprintf(path, "Saves/save_slot_%d.txt", slot);

    FILE *arquivo = fopen(path, "w");
    if (arquivo != NULL)
    {
        // 0. Metadados do Slot para carregamento rápido
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        char dateBuffer[32];
        strftime(dateBuffer, sizeof(dateBuffer), "%d/%m/%Y %H:%M", tm_info);

        fprintf(arquivo, "%s\n", (game->player.name[0] != '\0') ? game->player.name : "HERO");
        fprintf(arquivo, "%d\n", game->player.level);
        fprintf(arquivo, "%d\n", game->player.score);
        fprintf(arquivo, "%d\n", game->wave);
        fprintf(arquivo, "%s\n", dateBuffer);

        // 1. Dados do Jogador
        fprintf(arquivo, "%f\n", game->player.position.x);
        fprintf(arquivo, "%f\n", game->player.position.y);
        fprintf(arquivo, "%d\n", game->player.hp);
        fprintf(arquivo, "%d\n", game->player.maxHp);
        fprintf(arquivo, "%d\n", game->player.score);
        fprintf(arquivo, "%d\n", game->player.level);
        fprintf(arquivo, "%d\n", game->player.xp);
        fprintf(arquivo, "%d\n", game->player.xpNeeded);
        fprintf(arquivo, "%d\n", game->player.attackPower);
        fprintf(arquivo, "%f\n", game->player.speed);

        // 2. Estado do Mundo
        fprintf(arquivo, "%d\n", game->wave);
        fprintf(arquivo, "%d\n", game->totalEnemiesKilled);
        fprintf(arquivo, "%f\n", game->timeElapsed);

        // 3. Contagem e Estado dos Inimigos
        fprintf(arquivo, "%d\n", game->enemiesRemaining);

        // Escreve cada inimigo ativo
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (game->enemies[i].active)
            {
                fprintf(arquivo, "%f %f %d %d %d %f %d %d\n",
                        game->enemies[i].position.x,
                        game->enemies[i].position.y,
                        game->enemies[i].hp,
                        game->enemies[i].maxHp,
                        game->enemies[i].type,
                        game->enemies[i].speed,
                        game->enemies[i].tier,
                        game->enemies[i].isRanged);
            }
        }

        fclose(arquivo);
    }

    // Libera a memória do snapshot e dos parâmetros da thread
    free(game);
    free(data);
    _endthread();
}

void SalvarJogoSlot(GameState *game, int slot)
{
    // Efeito visual imediato na thread principal para dar feedback ao jogador
    SpawnParticleExplosion(game, game->player.position, GREEN, 15, 30.0f, 90.0f, 4.0f, 0.6f);

    // Snapshot para o Salvamento Assíncrono
    GameState *snapshot = (GameState *)malloc(sizeof(GameState));
    if (snapshot != NULL) {
        memcpy(snapshot, game, sizeof(GameState));
        
        SaveThreadData *data = (SaveThreadData *)malloc(sizeof(SaveThreadData));
        if (data != NULL) {
            data->gameSnapshot = snapshot;
            data->slot = slot;
            
            // Inicia a thread de salvamento em background usando _beginthread (C runtime)
            _beginthread(SaveGameThread, 0, data);
        } else {
            free(snapshot);
        }
    }
}

// ============================================================================
// PERSISTÊNCIA: CARREGAR JOGO POR SLOT
// ============================================================================
void CarregarJogoSlot(GameState *game, int slot)
{
    char path[64];
    sprintf(path, "Saves/save_slot_%d.txt", slot);

    FILE *arquivo = fopen(path, "r");
    if (arquivo != NULL)
    {
        // Reseta primeiro para evitar lixo nas partículas e power-ups
        float shakeOld = game->screenShake;
        GameScreen oldScreen = game->currentScreen;
        float tempVol = game->masterVolume;

        // Limpa estados de buffs temporários
        memset(game, 0, sizeof(GameState));

        game->currentScreen = oldScreen;
        game->screenShake = shakeOld;
        game->masterVolume = tempVol;

        // Pular/Ler metadados iniciais
        char nameLine[32];
        if (fgets(nameLine, sizeof(nameLine), arquivo) != NULL)
        {
            nameLine[strcspn(nameLine, "\r\n")] = '\0';
            strncpy(game->player.name, nameLine, 15);
            game->player.name[15] = '\0';
        }
        int dummyLevel, dummyScore, dummyWave;
        char dummyDate[32];
        fscanf(arquivo, "%d\n", &dummyLevel);
        fscanf(arquivo, "%d\n", &dummyScore);
        fscanf(arquivo, "%d\n", &dummyWave);
        if (fgets(dummyDate, sizeof(dummyDate), arquivo) != NULL)
        {
            // Apenas consome a linha da data
        }

        // 1. Dados do Jogador
        fscanf(arquivo, "%f\n", &game->player.position.x);
        fscanf(arquivo, "%f\n", &game->player.position.y);
        fscanf(arquivo, "%d\n", &game->player.hp);
        fscanf(arquivo, "%d\n", &game->player.maxHp);
        fscanf(arquivo, "%d\n", &game->player.score);
        fscanf(arquivo, "%d\n", &game->player.level);
        fscanf(arquivo, "%d\n", &game->player.xp);
        fscanf(arquivo, "%d\n", &game->player.xpNeeded);
        fscanf(arquivo, "%d\n", &game->player.attackPower);
        fscanf(arquivo, "%f\n", &game->player.speed);

        // 2. Estado do Mundo
        fscanf(arquivo, "%d\n", &game->wave);
        fscanf(arquivo, "%d\n", &game->totalEnemiesKilled);
        fscanf(arquivo, "%f\n", &game->timeElapsed);

        // 3. Contagem e Inimigos
        fscanf(arquivo, "%d\n", &game->enemiesRemaining);

        for (int i = 0; i < game->enemiesRemaining; i++)
        {
            if (i < MAX_ENEMIES)
            {
                int t = 0, isR = 0;
                fscanf(arquivo, "%f %f %d %d %d %f %d %d\n",
                        &game->enemies[i].position.x,
                        &game->enemies[i].position.y,
                        &game->enemies[i].hp,
                        &game->enemies[i].maxHp,
                        &game->enemies[i].type,
                        &game->enemies[i].speed,
                        &t,
                        &isR);
                game->enemies[i].tier = (EnemyTier)t;
                game->enemies[i].isRanged = (bool)isR;
                
                game->enemies[i].active = true;
                game->enemies[i].state = IDLE;
                game->enemies[i].patrolTarget = game->enemies[i].position;
                game->enemies[i].patrolTimer = 3.0f;
            }
        }

        // Câmera re-alinhada instantaneamente
        game->camera.target = game->player.position;
        game->camera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
        game->camera.zoom = 1.0f;
        game->camera.rotation = 0.0f;

        game->saveLoaded = true;
        fclose(arquivo);

        // Efeito visual de carregamento completo (Partículas ciano)
        SpawnParticleExplosion(game, game->player.position, SKYBLUE, 20, 50.0f, 150.0f, 4.0f, 0.7f);
    }
}

// ============================================================================
// PERSISTÊNCIA: CARREGAR METADADOS DE UM SLOT
// ============================================================================
SaveSlotMeta CarregarMetadadosSlot(int slot)
{
    SaveSlotMeta meta = { 0 };
    char path[64];
    sprintf(path, "Saves/save_slot_%d.txt", slot);

    FILE *arquivo = fopen(path, "r");
    if (arquivo != NULL)
    {
        meta.exists = true;
        
        // Linha 1: Nome do Jogador
        if (fgets(meta.name, sizeof(meta.name), arquivo) != NULL)
        {
            meta.name[strcspn(meta.name, "\r\n")] = '\0';
        }
        
        // Linha 2: Nível
        fscanf(arquivo, "%d\n", &meta.level);
        
        // Linha 3: Score
        fscanf(arquivo, "%d\n", &meta.score);
        
        // Linha 4: Wave
        fscanf(arquivo, "%d\n", &meta.wave);
        
        // Linha 5: Data
        if (fgets(meta.date, sizeof(meta.date), arquivo) != NULL)
        {
            meta.date[strcspn(meta.date, "\r\n")] = '\0';
        }
        
        fclose(arquivo);
    }
    else
    {
        meta.exists = false;
    }
    return meta;
}

// ============================================================================
// TELA DE CARREGAMENTO (LOADING SCREEN)
// ============================================================================
void RequestLoadingScreen(GameState *game, LoadTarget target, float duration)
{
    // Descarrega o mapa e entidades anteriores para otimização
    for (int i = 0; i < MAX_ENEMIES; i++) game->enemies[i].active = false;
    for (int i = 0; i < MAX_POWERUPS; i++) game->powerUps[i].active = false;
    for (int i = 0; i < MAX_PARTICLES; i++) game->particles[i].active = false;
    for (int i = 0; i < MAX_PROJECTILES; i++) game->projectiles[i].active = false;

    // Se estivermos saindo do tutorial, reseta inTutorial
    if (target == LOAD_TO_GAMEPLAY)
    {
        game->inTutorial = false;
    }

    game->currentScreen = SCREEN_LOADING;
    game->loadTarget = target;
    game->loadingTimer = 0.0f;
    game->loadingDuration = duration;
    game->loadingTip = GetRandomValue(0, 4);
}

void UpdateTelaLoading(GameState *game, float delta)
{
    game->loadingTimer += delta;

    // Emite algumas partículas decorativas biológicas/de rede na tela de carregamento
    if (GetRandomValue(0, 15) == 0)
    {
        Vector2 pos = { (float)GetRandomValue(0, SCREEN_WIDTH), SCREEN_HEIGHT + 10.0f };
        Vector2 vel = { (float)GetRandomValue(-20, 20), (float)GetRandomValue(-50, -20) };
        Color col = (GetRandomValue(0, 1) == 0) ? (Color){ 0, 229, 255, 255 } : (Color){ 0, 200, 100, 255 };
        SpawnParticle(game, pos, vel, col, (float)GetRandomValue(2, 4), (float)GetRandomValue(2, 4));
    }

    // Atualiza partículas decorativas da tela de loading
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (game->particles[i].active)
        {
            game->particles[i].position.y += game->particles[i].velocity.y * delta;
            game->particles[i].position.x += game->particles[i].velocity.x * delta;
            game->particles[i].lifeTime -= delta;
            if (game->particles[i].lifeTime <= 0.0f) game->particles[i].active = false;
        }
    }

    if (game->loadingTimer >= game->loadingDuration)
    {
        // Se houver slot de save especificado para carregar, carrega-o agora
        if (game->loadSlot > 0)
        {
            int slot = game->loadSlot;
            game->loadSlot = 0; // reseta
            CarregarJogoSlot(game, slot);
            game->currentScreen = SCREEN_GAMEPLAY;
            game->saveLoaded = true;
            strcpy(game->notificationMsg, "GAME LOADED!");
            game->timeElapsed = 0.0f;
            return;
        }

        // Caso contrário, faz o carregamento padrão do destino
        switch (game->loadTarget)
        {
            case LOAD_TO_TUTORIAL:
                InitTutorial(game);
                break;

            case LOAD_TO_GAMEPLAY:
                game->inTutorial = false;
                game->currentScreen = SCREEN_GAMEPLAY;
                // Posiciona jogador no centro do organismo
                game->player.position = (Vector2){ MAP_WIDTH / 2.0f, MAP_HEIGHT / 2.0f };
                // Reinicia a câmera
                game->camera.target = game->player.position;
                game->camera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
                game->camera.zoom = 1.0f;
                // Inicia wave
                StartNextWave(game);
                break;

            case LOAD_TO_MENU:
                game->currentScreen = SCREEN_MENU;
                break;

            case LOAD_TO_GAMEOVER:
                game->currentScreen = SCREEN_GAMEOVER;
                break;

            case LOAD_TO_VICTORY:
                game->currentScreen = SCREEN_VICTORY;
                break;
        }
    }
}

// ============================================================================
// TEXTOS DE DIÁLOGO DO TUTORIAL (PAGINADOS)
// ============================================================================
void GetTutorialDialogText(int step, int page, const char **line1, const char **line2, const char **line3)
{
    *line1 = "";
    *line2 = "";
    *line3 = "";
    if (step == 0)
    {
        if (page == 0)
        {
            *line1 = "Voce e um Anticorpo convocado para salvar o organismo de um humano infectado no DF.";
            *line2 = "Sua missao e combater patogenos e ensinar a populacao a prevenir doencas.";
            *line3 = "[Aperte 'Q' ou 'ESPACO' para continuar...]";
        }
        else
        {
            *line1 = "Voce foi gerado dentro de uma Seringa de Vacina, pronto para ser injetado.";
            *line2 = "Primeiro, vamos calibrar os biossensores e absorver doses iniciais de vacina.";
            *line3 = "Use WASD/Setas para se mover e colete as 3 ampolas de vacina no cenario.";
        }
    }
    else if (step == 1)
    {
        if (page == 0)
        {
            *line1 = "Alerta! Uma bacteria enfraquecida (vacina atenuada) foi inserida para treino!";
            *line2 = "Seu corpo precisa aprender a reconhecer e destruir essa ameaca.";
            *line3 = "[Aperte 'Q' ou 'ESPACO' para continuar...]";
        }
        else
        {
            *line1 = "Aproxime-se e use o BOTAO ESQUERDO DO MOUSE ou ESPACO para atacar.";
            *line2 = "Cuidado: ela persegue voce e dispara toxinas acidas. Esquive-se!";
            *line3 = "Ao ser eliminada, ela deixara uma cura. Colete-a para abrir o bocal!";
        }
    }
    else if (step == 2)
    {
        if (page == 0)
        {
            *line1 = "Excelente! Seu sistema imunologico registrou a assinatura desse patogeno.";
            *line2 = "Agora voce esta pronto para o combate real no corpo do paciente.";
            *line3 = "[Aperte 'Q' ou 'ESPACO' para continuar...]";
        }
        else
        {
            *line1 = "Va ate a ponta inferior da seringa (o bocal da agulha que esta piscando em verde).";
            *line2 = "Ao entrar no bocal, voce sera injetado na corrente sanguinea!";
            *line3 = "Siga em frente e salve o Distrito Federal!";
        }
    }
}

