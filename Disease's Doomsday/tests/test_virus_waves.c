// test_virus_waves.c
// ACEITAÇÃO (Mundo dos Vírus): valida os arquétipos virais e a composição das
// cinco ondas SEM janela gráfica. Linka wave_manager.c + enemy.c + map_body.c
// com stubs das funções de desenho/efeitos, então executa StartNextWave() de
// verdade e inspeciona game->enemies[].
//
// Cobre: IDs compatíveis; init de HP/escudo/velocidade/tier/comportamento;
// presença dos arquétipos por onda; chefe só na onda 5; sem chefe nas ondas 1-4;
// spawns dentro do corpo; e uma checagem de não-regressão do Mundo das Bactérias.
#include "raylib.h"
#include "../include/game.h"
#include "../include/gameplay.h"
#include "../src/systems/wave_manager.h"
#include "../Assets/Maps/map_body.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// ---- Stubs de desenho da raylib referenciados por map_body.c (não chamados) ----
void DrawLineEx(Vector2 a, Vector2 b, float t, Color c) { (void)a;(void)b;(void)t;(void)c; }
void DrawCircleV(Vector2 c, float r, Color col) { (void)c;(void)r;(void)col; }
void DrawCircleLines(int x, int y, float r, Color c) { (void)x;(void)y;(void)r;(void)c; }
void DrawEllipse(int x, int y, float rx, float ry, Color c) { (void)x;(void)y;(void)rx;(void)ry;(void)c; }
void DrawEllipseLines(int x, int y, float rx, float ry, Color c) { (void)x;(void)y;(void)rx;(void)ry;(void)c; }
void DrawTriangle(Vector2 a, Vector2 b, Vector2 cc, Color c) { (void)a;(void)b;(void)cc;(void)c; }
void DrawRectangleRounded(Rectangle r, float a, int s, Color c) { (void)r;(void)a;(void)s;(void)c; }
void DrawRectangleRoundedLines(Rectangle r, float a, int s, Color c) { (void)r;(void)a;(void)s;(void)c; }
void DrawTextEx(Font f, const char *t, Vector2 p, float s, float sp, Color c) { (void)f;(void)t;(void)p;(void)s;(void)sp;(void)c; }
Vector2 MeasureTextEx(Font f, const char *t, float s, float sp) { (void)f;(void)t;(void)s;(void)sp; return (Vector2){100,40}; }
Color Fade(Color c, float a) { (void)a; return c; }
int GetRandomValue(int mn, int mx) { return mn + rand() % (mx - mn + 1); }
bool SpriteAvailable(int id) { (void)id; return false; }
void DrawSpriteCentered(int id, Vector2 c, Vector2 s, float r, Color col) { (void)id;(void)c;(void)s;(void)r;(void)col; }
Texture2D GetSprite(int id) { (void)id; return (Texture2D){0}; }

// ---- Stubs dos efeitos/UI referenciados por wave_manager.c (sem efeito) ----
void SpawnParticle(GameState *g, Vector2 p, Vector2 v, Color c, float s, float l) { (void)g;(void)p;(void)v;(void)c;(void)s;(void)l; }
void SpawnPowerUpAt(GameState *g, Vector2 p, int t) { (void)g;(void)p;(void)t; }
void ShowBanner(GameState *g, const char *m, const char *s, Color c, float d) { (void)g;(void)m;(void)s;(void)c;(void)d; }
const char *TextFormat(const char *t, ...) { (void)t; return ""; }

static int fails = 0;
#define CHECK(cond, msg) do { if (!(cond)) { printf("  FALHA: %s\n", msg); fails++; } } while (0)

static void resetEnemies(GameState *g)
{
    for (int i = 0; i < MAX_ENEMIES; i++) g->enemies[i].active = false;
    g->enemiesRemaining = 0;
}

int main(void)
{
    srand(7);
    printf("=== TESTE DO MUNDO DOS VIRUS (arquetipos + ondas) ===\n");

    // ------------------------------------------------------------------
    // 1) COMPATIBILIDADE DOS IDs (legados preservados; novos no final)
    // ------------------------------------------------------------------
    CHECK(ETYPE_SARS == 0 && ETYPE_DENGUE_OLD == 1 && ETYPE_KPC == 2 && ETYPE_CHAGAS == 3 &&
          ETYPE_TB == 4 && ETYPE_BACT_MELEE == 5 && ETYPE_BACT_RANGED == 6 &&
          ETYPE_VIRUS_MELEE == 7 && ETYPE_VIRUS_RANGED == 8 && ETYPE_VIRUS_BOSS == 9 &&
          ETYPE_VIRUS_SWARM == 10 && ETYPE_VIRUS_ELITE == 11, "IDs dos tipos compativeis");
    for (int t = 0; t <= 11; t++)
    {
        const EnemyArchetype *a = EnemyArchetypeFor(t);
        CHECK(a != NULL && a->type == t, "arquetipo existe para cada ID");
    }

    // ------------------------------------------------------------------
    // 2) IDENTIDADES VIRAIS DISTINTAS (5) e comportamentos próprios
    // ------------------------------------------------------------------
    int viral[5] = { ETYPE_VIRUS_SWARM, ETYPE_VIRUS_MELEE, ETYPE_VIRUS_RANGED, ETYPE_VIRUS_ELITE, ETYPE_VIRUS_BOSS };
    EnemyBehavior expectBeh[5] = { BEHAV_SWARM, BEHAV_MELEE, BEHAV_RANGED, BEHAV_ELITE, BEHAV_BOSS };
    float sizes[5];
    for (int i = 0; i < 5; i++)
    {
        const EnemyArchetype *a = EnemyArchetypeFor(viral[i]);
        CHECK(a->behavior == expectBeh[i], "comportamento do arquetipo viral");
        sizes[i] = a->sizeScale;
    }
    // tamanhos variam de verdade (enxame < demais; elite e chefe maiores)
    CHECK(sizes[0] < 0.8f, "enxame e' pequeno");
    CHECK(sizes[3] > 1.3f, "elite e' grande");
    CHECK(sizes[4] > 1.0f, "chefe e' maior que o comum");

    // ------------------------------------------------------------------
    // 3) EnemyInitFromArchetype: HP, escudo, velocidade, tier, comportamento,
    //    e campos transitórios zerados (slot reuse-safe).
    // ------------------------------------------------------------------
    for (int i = 0; i < 4; i++) // comuns (não o chefe)
    {
        int type = viral[i];
        const EnemyArchetype *a = EnemyArchetypeFor(type);
        Enemy e; memset(&e, 0xAB, sizeof(e)); // suja o slot p/ provar a inicialização
        EnemyInitFromArchetype(&e, type, 3, 1.0f);
        CHECK(e.type == type, "init: type");
        CHECK(e.tier == a->tier, "init: tier");
        CHECK(e.isRanged == a->ranged, "init: isRanged casa com arquetipo");
        CHECK(e.hp == e.maxHp && e.maxHp == a->baseHp + 3 * a->hpPerWave, "init: HP base + escala por onda");
        CHECK(fabsf(e.speed - a->speed) < 0.01f, "init: velocidade");
        CHECK(e.shieldActive == (a->shieldBase > 0), "init: capsideo ativo sse shieldBase>0");
        CHECK(e.shieldHp == e.shieldMaxHp, "init: shieldHp==shieldMaxHp");
        if (a->shieldBase > 0)
            CHECK(e.shieldMaxHp == a->shieldBase + 3 * a->shieldPerWave, "init: escudo base + escala");
        CHECK(e.state == IDLE && !e.isTutorialEnemy, "init: estado IDLE, nao-tutorial");
        CHECK(e.poisonTimer == 0.0f && e.poisonAccum == 0.0f && e.slowTimer == 0.0f &&
              e.fleeTimer == 0.0f && e.summonTimer == 0.0f && e.hitCooldown == 0.0f &&
              e.aiPhase == 0 && e.spawnAnim == 0.0f && e.attackAnim == 0.0f &&
              e.aggroMemory == 0.0f && e.dodgeCooldown == 0.0f, "init: transitorios zerados");
    }
    // healthMul aplica
    {
        Enemy e; EnemyInitFromArchetype(&e, ETYPE_VIRUS_MELEE, 2, 2.0f);
        const EnemyArchetype *a = EnemyArchetypeFor(ETYPE_VIRUS_MELEE);
        CHECK(e.maxHp == (int)((a->baseHp + 2 * a->hpPerWave) * 2.0f), "init: healthMul escala HP");
    }

    // ------------------------------------------------------------------
    // 4) COMPOSIÇÃO PLANEJADA DAS ONDAS (distinct types + sacola + mini chefe)
    // ------------------------------------------------------------------
    {
        int t1[4], n1 = VirusWaveTypes(1, t1);
        CHECK(n1 == 1 && t1[0] == ETYPE_VIRUS_SWARM, "onda1: so' enxame (tipo basico)");
        int t2[4], n2 = VirusWaveTypes(2, t2);
        CHECK(n2 == 2 && t2[1] == ETYPE_VIRUS_MELEE, "onda2: + envelopado melee");
        int t3[4], n3 = VirusWaveTypes(3, t3);
        CHECK(n3 == 3 && t3[2] == ETYPE_VIRUS_RANGED, "onda3: + atirador");
        int t4[4], n4 = VirusWaveTypes(4, t4);
        CHECK(n4 == 4 && t4[3] == ETYPE_VIRUS_ELITE, "onda4: combinacao completa (inclui elite)");

        // mini chefe: introduz o elite na onda 3; nunca é o chefe final.
        CHECK(VirusMiniBossType(1) == ETYPE_VIRUS_SWARM, "minichefe onda1");
        CHECK(VirusMiniBossType(2) == ETYPE_VIRUS_MELEE, "minichefe onda2");
        CHECK(VirusMiniBossType(3) == ETYPE_VIRUS_ELITE, "minichefe onda3 = elite");
        CHECK(VirusMiniBossType(4) == ETYPE_VIRUS_ELITE, "minichefe onda4 = elite");
        for (int w = 1; w <= 4; w++) CHECK(VirusMiniBossType(w) != ETYPE_VIRUS_BOSS, "minichefe nunca e' o chefe");

        // sacola de spawn: contém cada tipo esperado (presença garantida) e nada
        // de elite antes da onda 4 nem atirador antes da onda 3.
        for (int w = 1; w <= 4; w++)
        {
            int bag[16]; int bn = VirusWaveBag(w, bag, 16);
            int et[4]; int en = VirusWaveTypes(w, et);
            for (int k = 0; k < en; k++)
            {
                int found = 0;
                for (int b = 0; b < bn; b++) if (bag[b] == et[k]) found = 1;
                CHECK(found, "sacola contem cada arquetipo esperado da onda");
            }
            if (w < 4) for (int b = 0; b < bn; b++) CHECK(bag[b] != ETYPE_VIRUS_ELITE, "sem elite comum antes da onda4");
            if (w < 3) for (int b = 0; b < bn; b++) CHECK(bag[b] != ETYPE_VIRUS_RANGED, "sem atirador antes da onda3");
            for (int b = 0; b < bn; b++) CHECK(bag[b] != ETYPE_VIRUS_BOSS, "sacola nunca tem o chefe");
        }
    }

    // ------------------------------------------------------------------
    // 5) INTEGRAÇÃO: StartNextWave() real, ondas 1..5 do Mundo dos Vírus
    // ------------------------------------------------------------------
    GameState *g = (GameState *)calloc(1, sizeof(GameState));
    g->currentWorld = WORLD_VIRUS;
    g->difficulty = DIFFICULTY_MEDIUM;
    g->diff.enemyHealthMul = 1.0f;
    g->diff.summonMul = 1.0f;
    g->player.position = MapBody_GetSafeCenter();

    for (int w = 1; w <= 5; w++)
    {
        resetEnemies(g);
        g->wave = w;
        StartNextWave(g);

        int active = 0, bossTier = 0, mini = 0, insideFail = 0;
        int typeSeen[12] = {0};
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (!g->enemies[i].active) continue;
            Enemy *e = &g->enemies[i];
            active++;
            if (e->type >= 0 && e->type < 12) typeSeen[e->type] = 1;
            if (e->tier == TIER_3_BOSS) bossTier++;
            if (e->tier == TIER_MINIBOSS) mini++;
            if (!MapBody_Contains(e->position)) insideFail++;
            CHECK(e->hp > 0 && e->maxHp > 0, "inimigo com HP valido");
        }
        CHECK(active > 0, "onda spawnou inimigos");
        CHECK(insideFail == 0, "todos os spawns dentro do corpo");

        if (w < 5)
        {
            CHECK(bossTier == 0, "ondas 1-4: nenhum CHEFE (TIER_3_BOSS)");
            CHECK(!typeSeen[ETYPE_VIRUS_BOSS], "ondas 1-4: nenhum tipo de chefe viral");
            CHECK(mini >= 1, "ondas 1-4: um mini chefe presente");
            int et[4]; int en = VirusWaveTypes(w, et);
            for (int k = 0; k < en; k++) CHECK(typeSeen[et[k]], "arquetipo esperado presente na onda");
            if (w >= 3) CHECK(typeSeen[ETYPE_VIRUS_ELITE], "elite presente a partir da onda 3 (mini chefe)");
            printf("  onda %d: ativos=%d  mini=%d  tipos[enx=%d mel=%d atr=%d eli=%d]\n",
                   w, active, mini, typeSeen[ETYPE_VIRUS_SWARM], typeSeen[ETYPE_VIRUS_MELEE],
                   typeSeen[ETYPE_VIRUS_RANGED], typeSeen[ETYPE_VIRUS_ELITE]);
        }
        else
        {
            CHECK(bossTier == 1, "onda 5: exatamente um CHEFE");
            CHECK(typeSeen[ETYPE_VIRUS_BOSS], "onda 5: chefe viral (coronavirus) presente");
            CHECK(mini == 0, "onda 5: sem mini chefe");
            CHECK(active > 1, "onda 5: chefe acompanhado de lacaios");
            printf("  onda 5: ativos=%d  chefes=%d (lacaios=%d)\n", active, bossTier, active - bossTier);
        }
    }

    // ------------------------------------------------------------------
    // 6) NÃO-REGRESSÃO DO MUNDO 1 (Bactérias): só tipos bacterianos, sem chefe
    //    na onda 1, com mini chefe; nenhum tipo viral vaza para o Mundo 1.
    // ------------------------------------------------------------------
    g->currentWorld = WORLD_BACTERIA;
    resetEnemies(g);
    g->wave = 1;
    StartNextWave(g);
    int bactBossTier = 0, bactMini = 0, viralLeak = 0, bactActive = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!g->enemies[i].active) continue;
        Enemy *e = &g->enemies[i];
        bactActive++;
        if (e->tier == TIER_3_BOSS) bactBossTier++;
        if (e->tier == TIER_MINIBOSS) bactMini++;
        if (e->type == ETYPE_VIRUS_MELEE || e->type == ETYPE_VIRUS_RANGED || e->type == ETYPE_VIRUS_BOSS ||
            e->type == ETYPE_VIRUS_SWARM || e->type == ETYPE_VIRUS_ELITE) viralLeak++;
    }
    CHECK(bactActive > 0 && bactBossTier == 0 && bactMini >= 1, "Mundo 1 onda1: sem chefe, com mini chefe");
    CHECK(viralLeak == 0, "Mundo 1 nao spawna tipos virais");

    free(g);
    printf("%s\n", fails == 0 ? "RESULTADO: PASSOU (arquetipos e composicao das ondas virais)"
                              : "RESULTADO: FALHOU");
    return fails == 0 ? 0 : 1;
}
