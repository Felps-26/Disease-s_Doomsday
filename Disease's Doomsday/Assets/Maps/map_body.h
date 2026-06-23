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

// ============================================================================
// CONSTANTES DE SPAWN/COLISÃO (centralizadas — evita números mágicos espalhados)
// Raios usados pela validação geométrica de spawns (player, inimigos, núcleos).
// ============================================================================
#define BODY_PLAYER_RADIUS   20.0f  // raio de colisão do herói
#define BODY_ENEMY_RADIUS     35.0f // raio do inimigo grande (KPC)
#define BODY_BOSS_RADIUS     100.0f // raio aproximado do chefe
#define MELEE_REACH          170.0f // alcance do golpe em área (slash 140 + folga 30)

// Margem (raio do "disco livre" exigido) para o spawn de um Núcleo de Infecção.
// Garante que o núcleo nasça inteiramente dentro do corpo, longe de paredes, e
// com espaço ao redor para o herói se posicionar e atacar corpo a corpo.
#define CORE_SPAWN_MARGIN     95.0f
#define CORE_BOSS_CLEARANCE  180.0f // distância mínima entre núcleo e centro do chefe
#define CORE_INTER_DISTANCE  240.0f // distância mínima entre núcleos

// ============================================================================
// TRANSFORMAÇÃO IMAGEM (corpo.png) -> MUNDO — FONTE ÚNICA DA VERDADE.
// Usada IGUALMENTE pelo renderer (DrawMapBody) e pelo baker da colisão
// (tools/bake_collision_mask.c), para que a área caminhável coincida EXATAMENTE
// com a silhueta desenhada. A imagem é desenhada PRESERVANDO A PROPORÇÃO
// (sem deformar), centrada no centro do mundo, com altura = MAP_HEIGHT * SCALE;
// a largura segue o aspecto real da textura. NÃO repita estes números em outro
// lugar — inclua este header.
// ============================================================================
#define MAPBODY_IMG_SCALE 1.00f   // altura da imagem como fração de MAP_HEIGHT
#define MAPBODY_IMG_DX    0.0f    // deslocamento horizontal do centro (px de mundo)
#define MAPBODY_IMG_DY    0.0f    // deslocamento vertical do centro (px de mundo)

// Parâmetros de GERAÇÃO da máscara de colisão (baker offline; ver Makefile:
// `make collision-mask`). Não são usados em tempo de execução.
#define MAPBODY_BAKE_GN     452   // resolução da grade (célula = MAP_WIDTH/GN ≈ 12.5 px; escala com o mundo)
#define MAPBODY_BAKE_LUMA   12    // luminância mínima (0-255): pernas escuras ~12-21, fundo ~6-9
#define MAPBODY_BAKE_CLOSE  1     // fechamento morfológico mínimo: só sela anti-aliasing nas juntas
                                  // (1 em vez de 4 reduz ~4.6x a área jogável sobre o vazio visual,
                                  //  mantendo membros conectados — ver métrica "vazio-externo" do baker)

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
// true se um DISCO de raio `margin` ao redor de p está inteiramente dentro do
// corpo (condição suficiente, conservadora: cabe numa única cápsula). Usado
// para validar spawns longe das paredes e em passagens largas o bastante.
bool MapBody_ContainsWithMargin(Vector2 p, float margin);
// Mantém uma entidade de raio `radius` DENTRO do corpo: se estiver fora, empurra
// para a borda interna mais próxima (corrige pos in-place).
void MapBody_ApplyCollision(Vector2 *pos, float radius);
// Retorna um ponto aleatório dentro do corpo, longe de `avoid` (p/ spawns).
Vector2 MapBody_RandomPointInside(Vector2 avoid, float minDistFromAvoid);

// ---- Helpers de spawn reutilizáveis (chefe, lacaios, núcleos, power-ups) ----
// Centro seguro do tórax (ponto de folga máxima usado como fallback determinístico).
Vector2 MapBody_GetSafeCenter(void);
// Caixa delimitadora (px de mundo) da silhueta do corpo — enquadra o minimapa.
Rectangle MapBody_WorldBounds(void);
// Procura, de forma DETERMINÍSTICA, um ponto com "disco livre" de raio `margin`
// próximo de `preferred`; se falhar, caminha em direção a `fallback` e por fim
// usa o centro seguro do tórax. Retorna true se encontrou (out preenchido).
bool MapBody_FindClearPoint(Vector2 preferred, Vector2 fallback, float margin, Vector2 *out);
// Conveniência: valida/encontra um ponto de spawn com a margem pedida, usando o
// centro do tórax como fallback. Sempre retorna um ponto utilizável.
Vector2 MapBody_FindSpawnPoint(Vector2 preferred, float margin);
// Posiciona até `maxCores` Núcleos de Infecção ao redor de `bossCenter`, todos
// inteiramente dentro do corpo (margem), longe do chefe e entre si, alcançáveis
// por melee. Determinístico, com fallback central. Retorna quantos posicionou.
int MapBody_PlaceCores(Vector2 bossCenter, Vector2 *out, int maxCores,
                       float coreMargin, float bossClear, float interCore);

#endif // MAP_BODY_H
