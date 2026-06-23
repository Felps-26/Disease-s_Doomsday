#include "enemy.h"
#include <stddef.h> // NULL

// ============================================================================
// ARQUÉTIPOS DE PATÓGENO — configuração centralizada (tabela única)
// ----------------------------------------------------------------------------
// Toda a configuração de stats/visual/comportamento dos inimigos vive aqui. O
// wave manager inicializa inimigos comuns via EnemyInitFromArchetype(); a IA lê
// `behavior`; o renderer lê `sizeScale`/`palette`. Os IDs legados (0..4) e o
// chefe (gerenciado por ConfigureBoss) também têm entradas para que consultas de
// visual/afinidade nunca retornem NULL. Sem números mágicos espalhados.
//
// Os valores base de HP/velocidade/capsídeo dos tipos JÁ usados em campanha
// (bactérias e vírus melee/ranged) reproduzem exatamente a balança anterior —
// a dificuldade global continua sendo aplicada pelo wave manager.
// ============================================================================
static const EnemyArchetype ARCHETYPES[] = {
    // --- Legados (compatibilidade; não spawnam na campanha de 2 Mundos) ---
    { ETYPE_SARS,       "SARS-CoV-2",  BEHAV_MELEE,  TIER_1,      false,  28,  8, 150.0f,  0,  0, 1.00f, { 140, 50, 200, 255 }, 0 },
    { ETYPE_DENGUE_OLD, "Aedes",       BEHAV_RANGED, TIER_2,      true,   22,  6, 180.0f,  0,  0, 1.00f, {  80, 80,  80, 255 }, 4 },
    { ETYPE_CHAGAS,     "T. cruzi",    BEHAV_MELEE,  TIER_1,      false,  26,  8, 175.0f,  0,  0, 1.00f, {  30,100, 220, 255 }, 0 },
    { ETYPE_TB,         "Tuberculose", BEHAV_RANGED, TIER_3,      true,  150, 30, 100.0f,  0,  0, 1.00f, { 180, 60,  40, 255 }, 0 },

    // --- Mundo 1: Bactérias (valores e TAMANHOS idênticos aos atuais: scale 1.0) ---
    { ETYPE_BACT_MELEE,  "Coco",       BEHAV_MELEE,  TIER_1,      false,  32, 10, 150.0f,  0,  0, 1.00f, {  90, 200, 120, 255 }, 0 },
    { ETYPE_BACT_RANGED, "Bacilo",     BEHAV_RANGED, TIER_2,      true,   24,  6, 110.0f,  0,  0, 1.00f, { 170, 200,  70, 255 }, 0 },
    { ETYPE_KPC,         "Superbacteria KPC", BEHAV_RANGED, TIER_3, true, 200, 40, 60.0f,  0,  0, 1.00f, {  50, 200,  80, 255 }, 14 },

    // --- Mundo 2: Vírus (5 identidades distintas, com capsídeo variável) ---
    // 1) Enxame (rinovírus): pequeno, veloz, frágil, capsídeo fraco, numeroso.
    { ETYPE_VIRUS_SWARM,  "Rinovirus",  BEHAV_SWARM,  TIER_1,      false,  12,  3, 275.0f,  8,  2, 0.60f, { 210, 220,  90, 255 }, 0 },
    // 2) Envelopado corpo a corpo (dengue): médio, investida, dano de contato.
    { ETYPE_VIRUS_MELEE,  "Dengue",     BEHAV_MELEE,  TIER_1,      false,  30,  9, 160.0f, 28,  8, 1.05f, { 230,  90,  90, 255 }, 3 },
    // 3) Atirador (influenza): mantém distância, dispara material viral, recua.
    { ETYPE_VIRUS_RANGED, "Influenza",  BEHAV_RANGED, TIER_2,      true,   22,  6, 200.0f, 28,  8, 1.00f, { 240, 160,  60, 255 }, 0 },
    // 4) Elite/mutante (sarampo): grande, capsídeo reforçado, alterna comportamento.
    { ETYPE_VIRUS_ELITE,  "Sarampo (mutante)", BEHAV_ELITE, TIER_3, true, 180, 30, 95.0f, 90, 20, 1.50f, { 180,  70, 200, 255 }, 10 },
    // 5) Chefe (coronavírus): stats nominais; ConfigureBoss faz o escalonamento.
    //    sizeScale 1.15 -> chefe viral significativamente maior que os demais.
    { ETYPE_VIRUS_BOSS,   "Coronavirus",BEHAV_BOSS,   TIER_3_BOSS, true, 1400,100, 70.0f,  0,  0, 1.15f, { 200,  60, 160, 255 }, 0 },
};
static const int ARCHETYPE_COUNT = (int)(sizeof(ARCHETYPES) / sizeof(ARCHETYPES[0]));

const EnemyArchetype *EnemyArchetypeFor(int type)
{
    for (int i = 0; i < ARCHETYPE_COUNT; i++)
        if (ARCHETYPES[i].type == type) return &ARCHETYPES[i];
    return NULL;
}

void EnemyInitFromArchetype(Enemy *e, int type, int wave, float healthMul)
{
    const EnemyArchetype *a = EnemyArchetypeFor(type);
    if (a == NULL) a = EnemyArchetypeFor(ETYPE_BACT_MELEE); // fallback seguro

    e->type     = a->type;
    e->tier     = a->tier;
    e->isRanged = a->ranged;

    int hp = a->baseHp + wave * a->hpPerWave;
    if (healthMul > 0.01f) hp = (int)(hp * healthMul);
    if (hp < 1) hp = 1;
    e->maxHp = hp;
    e->hp    = hp;
    e->speed = a->speed;

    e->state         = IDLE;
    e->patrolTimer   = 3.0f;
    e->cooldownTimer = 1.5f;
    e->chargeTimer   = 0.0f;
    e->poisonTimer   = 0.0f;
    e->poisonAccum   = 0.0f;
    e->slowTimer     = 0.0f;
    e->isTutorialEnemy = false;

    e->flankSign   = 0.0f;   // o chamador pode randomizar o lado
    e->fleeTimer   = 0.0f;
    e->isEscort    = false;
    e->aiPhase     = 0;
    e->summonTimer = 0.0f;
    e->hitCooldown = 0.0f;

    e->lastKnownPlayerPos = (Vector2){ 0.0f, 0.0f };
    e->aggroMemory   = 0.0f;
    e->dodgeCooldown = 0.0f;

    // Capsídeo (escudo) — derivado do arquétipo (não escala com dificuldade).
    int sh = a->shieldBase + wave * a->shieldPerWave;
    if (sh > 0) { e->shieldMaxHp = sh; e->shieldHp = sh; e->shieldActive = true; }
    else        { e->shieldMaxHp = 0;  e->shieldHp = 0;  e->shieldActive = false; }
    e->shieldHitFlash = 0.0f;

    // Animação procedural transitória (slot reuse-safe).
    e->velSmooth  = (Vector2){ 0.0f, 0.0f };
    e->animTime   = 0.0f;
    e->attackAnim = 0.0f;
    e->spawnAnim  = 0.0f;

    // position/active/patrolTarget ficam a cargo do código de spawn.
    e->patrolTarget = (Vector2){ 0.0f, 0.0f };
}

// ----------------------------------------------------------------------------
// PROGRESSÃO DAS ONDAS VIRAIS — composição planejada por onda.
//   Onda 1: enxame (tipo básico).
//   Onda 2: + envelopado corpo a corpo (segunda função).
//   Onda 3: + atirador (mistura) e o ELITE chega como mini chefe.
//   Onda 4: combinação completa (enxame+melee+ranged+elite), prepara o chefe.
//   Onda 5: chefe viral exclusivo (tratado pelo wave manager).
// ----------------------------------------------------------------------------
int VirusWaveTypes(int wave, int *typesOut)
{
    int n = 0;
    typesOut[n++] = ETYPE_VIRUS_SWARM;                 // base, sempre presente
    if (wave >= 2) typesOut[n++] = ETYPE_VIRUS_MELEE;
    if (wave >= 3) typesOut[n++] = ETYPE_VIRUS_RANGED;
    if (wave >= 4) typesOut[n++] = ETYPE_VIRUS_ELITE;
    return n;
}

int VirusWaveBag(int wave, int *out, int cap)
{
    // Sacolas ordenadas por onda. Cada uma contém todos os tipos de
    // VirusWaveTypes(wave) (presença garantida nos primeiros slots) e pondera as
    // proporções: muito enxame/melee, pouco elite. O spawn usa out[idx % n].
    static const int W1[] = { ETYPE_VIRUS_SWARM };
    static const int W2[] = { ETYPE_VIRUS_SWARM, ETYPE_VIRUS_MELEE, ETYPE_VIRUS_SWARM };
    static const int W3[] = { ETYPE_VIRUS_SWARM, ETYPE_VIRUS_MELEE, ETYPE_VIRUS_RANGED,
                              ETYPE_VIRUS_SWARM, ETYPE_VIRUS_MELEE };
    static const int W4[] = { ETYPE_VIRUS_SWARM, ETYPE_VIRUS_MELEE, ETYPE_VIRUS_RANGED, ETYPE_VIRUS_ELITE,
                              ETYPE_VIRUS_SWARM, ETYPE_VIRUS_MELEE, ETYPE_VIRUS_RANGED, ETYPE_VIRUS_SWARM };
    const int *src; int n;
    if (wave <= 1)      { src = W1; n = (int)(sizeof(W1) / sizeof(W1[0])); }
    else if (wave == 2) { src = W2; n = (int)(sizeof(W2) / sizeof(W2[0])); }
    else if (wave == 3) { src = W3; n = (int)(sizeof(W3) / sizeof(W3[0])); }
    else                { src = W4; n = (int)(sizeof(W4) / sizeof(W4[0])); }
    if (n > cap) n = cap;
    for (int i = 0; i < n; i++) out[i] = src[i];
    return n;
}

int VirusMiniBossType(int wave)
{
    if (wave <= 1) return ETYPE_VIRUS_SWARM; // líder de enxame
    if (wave == 2) return ETYPE_VIRUS_MELEE; // bruto envelopado
    return ETYPE_VIRUS_ELITE;                // ondas 3-4: mutante
}

const char *BossDisplayName(int currentWorld)
{
    // WORLD_VIRUS == 1 (ver game.h). Mantém este módulo independente de game.h.
    return (currentWorld == 1) ? "CORONAVIRUS - CAPSIDEO REFORCADO"
                               : "SUPERBACTERIA KPC - RESISTENTE";
}
