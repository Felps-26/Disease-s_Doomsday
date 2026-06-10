// game.h
// Definições de constantes, enums e estruturas do RPG.
// Disease's Doomsday — Projeto de Saúde Pública / DF
#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <stdbool.h>

// ============================================================================
// CONSTANTES DO JOGO
// ============================================================================
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MAP_WIDTH 4000
#define MAP_HEIGHT 4000

// ============================================================================
// CORES GLOBAIS DO TEMA
// ============================================================================
#define THEME_COLOR_MAIN (Color){ 0, 229, 255, 255 }     // Cyan brilhante para bordas e destaques
#define THEME_COLOR_TEXT (Color){ 80, 220, 120, 255 }    // Verde biológico para textos de título
#define THEME_COLOR_BG_DARK (Color){ 6, 18, 10, 255 }    // Verde-petóleo escuro (fundo)
#define THEME_COLOR_BG_LIGHT (Color){ 10, 28, 18, 255 }  // Verde-petóleo um pouco mais claro
#define THEME_COLOR_PANEL (Color){ 14, 10, 26, 255 }     // Cor base para painéis/fundos transparentes
#define THEME_COLOR_BORDER (Color){ 84, 52, 148, 255 }   // Cor base para bordas padrão


// Dimensões do mapa do Tutorial: interior de uma Seringa de Vacina
#define SYRINGE_WIDTH  1200
#define SYRINGE_HEIGHT 1200

#define MAX_ENEMIES 60
#define MAX_POWERUPS 20
#define MAX_PARTICLES 250
#define MAX_PROJECTILES 100

// ============================================================================
// ENUMS
// ============================================================================
typedef enum GameScreen
{
    SCREEN_MENU,
    SCREEN_CONTROLS,
    SCREEN_GAMEPLAY,
    SCREEN_PAUSE,
    SCREEN_GAMEOVER,
    SCREEN_VICTORY,
    SCREEN_SAVE_SELECT,
    SCREEN_LOAD_SELECT,
    SCREEN_SETTINGS,
    SCREEN_TUTORIAL,   // Tutorial inicial: interior da Seringa de Vacina
    SCREEN_LOADING,    // Tela de carregamento entre transições de cena
    SCREEN_QUIZ,       // Tela de quiz educacional (DF)
    SCREEN_UPGRADE     // Tela de upgrade do SUS
} GameScreen;

// Destino após a tela de carregamento completar
typedef enum LoadTarget
{
    LOAD_TO_TUTORIAL,
    LOAD_TO_GAMEPLAY,
    LOAD_TO_MENU,
    LOAD_TO_GAMEOVER,
    LOAD_TO_VICTORY
} LoadTarget;

// Sistema de Diálogo do Tutorial (Typewriter + Q para avançar)
typedef struct DialogState
{
    bool  active;       // true = diálogo visível bloqueando gameplay
    int   page;         // página atual dentro do passo
    int   charShown;    // número de caracteres já revelados (efeito typewriter)
    float charTimer;    // acumulador de tempo para revelar próximo caractere
} DialogState;

typedef enum EnemyState
{
    IDLE,
    AGGRO,
    ATTACK,
    HURT,
    DEATH
} EnemyState;

typedef enum EnemyTier
{
    TIER_1,
    TIER_2,
    TIER_3,
    TIER_3_BOSS
} EnemyTier;

typedef enum ProjectileType
{
    PROJ_ACID_ARC,          // Ácido bacteriano (Tier 1)
    PROJ_BULLET_SPREAD,     // Picada espalhada do Aedes aegypti (Tier 2)
    PROJ_VOID_BOLT,         // Toxina viral concentrada (Tier 3 / Elite)
    PROJ_BOSS_BULLET        // Projétil da Superbactéria Resistente (Boss)
} ProjectileType;

typedef enum PowerUpType
{
    HP_RECOVERY,
    SPEED_BOOST,
    SHIELD,
    ATTACK_BOOST
} PowerUpType;

// ============================================================================
// ESTRUTURAS DE ENTIDADES
// ============================================================================
typedef struct Player
{
    char name[16];        // Nome do jogador
    Vector2 position;
    float speed;
    int hp;
    int maxHp;
    int score;
    int level;
    int xp;
    int xpNeeded;
    int attackPower;
    int susPoints; // Pontos para upgrades (Pontos do SUS)
    
    // Timers de Buffs e Debuffs (ativos se > 0)
    float speedTimer;
    float shieldTimer;
    float attackBoostTimer;
    float poisonTimer; // Dano ao longo do tempo
    float slowTimer;   // Redução de velocidade
    
    // Combate
    float attackCooldown; // Tempo até o próximo ataque
    
    // Animação
    int facingDir; // 1 para Direita, -1 para Esquerda
    bool isMoving;

} Player;

typedef struct Enemy
{
    Vector2 position;
    float speed;
    int hp;
    int maxHp;
    EnemyState state;
    Vector2 patrolTarget;
    float patrolTimer;
    // Tipos de Patógenos:
    // 0 = Bactéria Comum / Vírus da Gripe (Influenza) — TIER_1
    // 1 = Mosquito Aedes aegypti (Dengue, ranged) — TIER_2
    // 2 = Superbactéria Resistente / Infecção Grave (Elite/Boss) — TIER_3/BOSS
    int type;
    EnemyTier tier;
    bool active;     // Ativo/vivo no jogo
    bool isTutorialEnemy; // Inimigo de treino do tutorial (não ataca)
    
    
    // Ranged
    float cooldownTimer;
    float chargeTimer;
    bool isRanged;
} Enemy;

typedef struct PowerUp
{
    Vector2 position;
    PowerUpType type;
    bool active;
    float pulseTimer; // Para efeito visual pulsante no HUD e mapa
} PowerUp;

typedef struct Particle
{
    Vector2 position;
    Vector2 velocity;
    Color color;
    float size;
    float lifeTime;
    float maxLifeTime;
    bool active;
} Particle;

typedef struct Projectile
{
    Vector2 position;
    Vector2 velocity;
    bool active;
    ProjectileType type;
    int damage;
    Rectangle hitbox;
    
} Projectile;

// ============================================================================
// ESTRUTURAS DE UI
// ============================================================================
typedef struct UIButton
{
    Rectangle bounds;
    const char *text;
    bool hover;
    bool clicked;
} UIButton;

// ============================================================================
// METADADOS DO SLOT DE SAVE
// ============================================================================
typedef struct SaveSlotMeta
{
    bool exists;
    char name[16];
    int level;
    int score;
    int wave;
    char date[32];
} SaveSlotMeta;

// ============================================================================
// ESTADO GLOBAL DO JOGO
// ============================================================================
typedef struct GameState
{
    GameScreen currentScreen;
    Player player;
    Enemy enemies[MAX_ENEMIES];
    PowerUp powerUps[MAX_POWERUPS];
    Particle particles[MAX_PARTICLES];
    Projectile projectiles[MAX_PROJECTILES];
    
    int totalEnemiesKilled;
    int enemiesRemaining;
    int wave;
    Camera2D camera;
    
    // Sistema
    bool saveLoaded;
    char notificationMsg[32];
    float timeElapsed;
    float screenShake;
    
    // Controle de Input do Nome
    bool nameInputActive;
    
    // Animação de ataque (Slash)
    float slashAnimTimer;
    Vector2 slashAnimPos;
    float slashAnimRadius;

    // Metadados dos slots carregados na tela de seleção
    SaveSlotMeta slotsMeta[3];
    
    // Configurações
    float masterVolume;

    // ---- Estado do Tutorial (Seringa de Vacina) ----
    bool inTutorial;            // true = jogador está dentro da seringa
    int  tutorialStep;          // 0=Movimento, 1=Combate, 2=Saída pela agulha
    float tutorialTimer;        // Cronômetro acumulador para o passo de movimento
    bool tutorialEnemySpawned;  // Garante que apenas 1 bactéria tutorial seja criada
    DialogState tutorialDialog; // Estado do sistema de diálogo do tutorial

    // ---- Tela de Carregamento ----
    LoadTarget  loadTarget;      // Para onde ir após o loading terminar
    float       loadingTimer;    // Tempo decorrido na tela de loading
    float       loadingDuration; // Duração total da tela de loading (segundos)
    int         loadingTip;      // Índice da dica educativa atual
    int         loadSlot;        // Slot de save para carregar após o loading (0 = não carrega)
} GameState;

#endif // GAME_H
