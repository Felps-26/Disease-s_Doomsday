// map_organismo.h
// Mapa Principal: Interior do Organismo Humano (Campo de Batalha)
// Disease's Doomsday — Projeto de Saúde Pública / DF
#ifndef MAP_ORGANISMO_H
#define MAP_ORGANISMO_H

#include "raylib.h"
#include "../../include/game.h"

// ============================================================================
// CONSTANTES DO MAPA DO ORGANISMO
// ============================================================================

#define ORG_GRID_SPACING   120     // Espaçamento da grade interna
#define ORG_BORDER_THICK   15.0f   // Espessura da borda do mapa
#define ORG_WARNING_STEP   300     // Intervalo dos textos de aviso na borda

// Cor base do fundo (verde-petróleo biológico escuro)
#define ORG_BG_COLOR_TOP   ((Color){ 6,  14,  8, 255 })
#define ORG_BG_COLOR_BOT   ((Color){ 4,  10,  6, 255 })

// ============================================================================
// API DO MAPA
// ============================================================================

// Desenha o mapa completo do Organismo (chamar dentro de BeginMode2D)
// isBossFight: true = muda paleta para vermelho/púrpura (onda do Boss)
// bgColor / gridColor: cores calculadas pelo rpg.c com base no estado da onda
void DrawMapOrganismo(Font font, bool isBossFight, Color bgColor, Color gridColor);

#endif // MAP_ORGANISMO_H
