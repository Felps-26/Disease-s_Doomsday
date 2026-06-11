// gameplay.h
// Declarações das funções de física, combate, IA e persistência do RPG.
#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "game.h"

// Inicializa ou reseta o estado do jogo
void InitGame(GameState *game);

// Inicia a próxima horda/onda de inimigos
void StartNextWave(GameState *game);

// Executa um ciclo de lógica do jogo (movimentação, IA, física, colisão)
void UpdateGameplay(GameState *game, float delta);

// Executa o ataque em área do jogador (Space / Clique)
void PlayerAttack(GameState *game, Vector2 worldMousePos);

// Cria uma partícula no mundo com parâmetros definidos
void InitParticlePool(GameState *game);
void FreeParticle(GameState *game, int idx);
void SpawnParticle(GameState *game, Vector2 position, Vector2 velocity, Color color, float size, float lifeTime);
void SpawnParticleExplosion(GameState *game, Vector2 pos, Color col, int count, float minSpeed, float maxSpeed, float size, float life);

// Spawna um power-up em uma posição específica (por exemplo, após derrotar um inimigo)
void SpawnPowerUpAt(GameState *game, Vector2 position, int type);

// Salva o progresso do jogo atual em um slot específico (1, 2 ou 3)
void SalvarJogoSlot(GameState *game, int slot);

// Carrega o progresso do jogo a partir de um slot específico (1, 2 ou 3)
void CarregarJogoSlot(GameState *game, int slot);

// Carrega apenas os metadados de um slot específico
SaveSlotMeta CarregarMetadadosSlot(int slot);

// Cria um projétil no mundo disparado por um inimigo
void SpawnProjectile(GameState *game, Vector2 pos, Vector2 target, ProjectileType type, int dmg);

// Inicia uma tela de carregamento e define o destino após completar
// duration: tempo em segundos que a tela de loading fica visível
void RequestLoadingScreen(GameState *game, LoadTarget target, float duration);

// Inicializa o modo Tutorial (posiciona jogador na Seringa de Vacina)
void InitTutorial(GameState *game);

// Executa a lógica de atualização do Tutorial (movimento, combate, saída)
void UpdateTutorial(GameState *game, float delta);

// Executa a lógica da tela de carregamento (timer + transição de cena)
void UpdateTelaLoading(GameState *game, float delta);

// Obtém as linhas de texto para um determinado passo e página do diálogo do tutorial
void GetTutorialDialogText(int step, int page, const char **line1, const char **line2, const char **line3);

// ---- Feedback de combate ----
// Cria um número de dano flutuante na posição informada
void SpawnDamageText(GameState *game, Vector2 pos, int value, Color color);
// Atualiza (sobe/some) os números de dano ativos
void UpdateDamageTexts(GameState *game, float delta);

// ---- Skins ----
// Cores da skin de arma atual (primária = projétil/lâmina, secundária = brilho/trail)
Color WeaponSkinPrimary(int weaponSkinId);
Color WeaponSkinSecondary(int weaponSkinId);
// Nomes legíveis para a UI
const char *PlayerSkinName(int skinId);
const char *WeaponSkinName(int weaponSkinId);

// ---- Configuração persistente (volume + skins) ----
void LoadPlayerConfig(GameState *game);
void SavePlayerConfig(GameState *game);

#endif // GAMEPLAY_H
