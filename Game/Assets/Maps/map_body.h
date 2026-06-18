// map_body.h
// Mapa principal: Corpo Humano (silhueta + órgãos-alvo)
// Disease's Doomsday — Projeto de Saúde Pública / DF
//
// Substitui/expande a arena genérica do organismo por um corpo humano com
// silhueta e órgãos-alvo rotulados, coerentes com as doenças do escopo:
//   - Pulmões            -> pneumonia bacteriana (Mundo 1) e influenza (Mundo 2)
//   - Corrente sanguínea -> dengue (Mundo 2) e disseminação geral
//   - Foco hospitalar    -> superbactéria / exposição hospitalar (chefe do Mundo 1)
//
// Desenhado em coordenadas de MUNDO (0..MAP_WIDTH x 0..MAP_HEIGHT), dentro de
// BeginMode2D. Respeita a pipeline de sprites (Fase 1): se houver PNGs em
// Assets/Sprites/Map/, usa-os; senão, desenha proceduralmente.
#ifndef MAP_BODY_H
#define MAP_BODY_H

#include "raylib.h"
#include "../../include/game.h"

// Identificadores de região/órgão-alvo
typedef enum BodyRegion
{
    REGION_LUNGS = 0,     // Pulmões (vias respiratórias)
    REGION_BLOODSTREAM,   // Corrente sanguínea / vasos
    REGION_HOSPITAL_FOCUS // Foco/ambiente hospitalar
} BodyRegion;

// Desenha a silhueta do corpo e os órgãos-alvo (chamar dentro de BeginMode2D).
// currentWorld destaca os órgãos relevantes; wave permite enfatizar o alvo da
// onda atual; time alimenta animações (pulsação dos órgãos em foco). font é
// usada para os rótulos dos órgãos desenhados no mundo.
void DrawMapBody(Font font, int currentWorld, int wave, float time);

// Região/órgão em foco para o Mundo e onda atuais (usado p/ HUD e spawn).
BodyRegion MapBody_GetFocusRegion(int currentWorld, int wave);

// Centro (em coordenadas de mundo) do órgão-alvo da onda atual — as ondas
// podem acontecer "sobre" o órgão correspondente.
Vector2 MapBody_GetRegionCenter(BodyRegion region);

// Rótulos curtos para o HUD (reforço educativo): nome do órgão/região e da
// doença sendo combatida no Mundo + onda atuais.
const char *MapBody_GetRegionLabel(BodyRegion region);
const char *MapBody_GetDiseaseLabel(int currentWorld, int wave);

// ---- Colisão com o corpo (o corpo É a área jogável) ----
// true se o ponto p está dentro da silhueta do corpo.
bool MapBody_Contains(Vector2 p);
// Mantém uma entidade de raio `radius` DENTRO do corpo: se estiver fora, empurra
// para a borda interna mais próxima (corrige pos in-place).
void MapBody_ApplyCollision(Vector2 *pos, float radius);
// Retorna um ponto aleatório dentro do corpo, longe de `avoid` (p/ spawns).
Vector2 MapBody_RandomPointInside(Vector2 avoid, float minDistFromAvoid);

#endif // MAP_BODY_H
