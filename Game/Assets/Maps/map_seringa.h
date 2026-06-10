// map_seringa.h
// Mapa do Tutorial: Interior da Seringa de Vacina
// Disease's Doomsday — Projeto de Saúde Pública / DF
#ifndef MAP_SERINGA_H
#define MAP_SERINGA_H

#include "raylib.h"
#include "../../include/game.h"

// ============================================================================
// CONSTANTES GEOMÉTRICAS DA SERINGA
// ============================================================================

#define SYR_WALL_LEFT    80.0f
#define SYR_WALL_RIGHT   (SYRINGE_WIDTH  - 80.0f)
#define SYR_TOP          60.0f
#define SYR_BOTTOM       (SYRINGE_HEIGHT - 40.0f)
#define SYR_TAPER_Y      (SYRINGE_HEIGHT * 0.65f)   // Início do afunilamento

// Bocal / agulha (ponto de saída)
#define SYR_NEEDLE_X     (SYRINGE_WIDTH  / 2.0f - 30.0f)
#define SYR_NEEDLE_W     60.0f
#define SYR_NEEDLE_H     60.0f

// Retângulo de gatilho de saída (colisão para avançar ao organismo)
#define SYR_EXIT_X       (SYRINGE_WIDTH  / 2.0f - 55.0f)
#define SYR_EXIT_Y       (SYRINGE_HEIGHT - 80.0f)
#define SYR_EXIT_W       110.0f
#define SYR_EXIT_H       80.0f

// ============================================================================
// API DO MAPA
// ============================================================================

// Desenha o interior completo da Seringa (deve ser chamado dentro de BeginMode2D)
// tutorialStep: 0=movimento, 1=combate, 2=saída  |  time: GetTime() para animações
void DrawMapSeringa(Font font, int tutorialStep, float time);

// Retorna true se o playerRect colidiu com o bocal de saída da seringa
bool MapSeringa_CheckExit(Rectangle playerRect);

// Aplica colisão das paredes do cilindro (corrige playerPos in-place)
// playerRadius: raio de colisão circular do jogador
void MapSeringa_ApplyWallCollision(Vector2 *playerPos, float playerRadius);

#endif // MAP_SERINGA_H
