// telas.h
// Declarações das funções de desenho de menus, HUD, minimapa e botões interativos.
#ifndef TELAS_H
#define TELAS_H

#include "game.h"

// Desenha um botão UIButton com o estilo Premium da interface
void DrawButton(UIButton botao, Font font, bool enabled);

// Desenha texto centralizado que reduz a fonte para caber na largura da área.
void DrawTextFitCentered(Font font, const char *text, Rectangle area, float maxFont, Color color, bool shadow);

// Verifica (com cache de ~1s) se existe algum arquivo de save nos 3 slots.
// Evita abrir arquivos do disco a cada frame no menu.
bool AnySaveExistsCached(void);

void DrawSciFiBox(Rectangle r, Color col);

// Funções de desenho das respectivas telas
void DrawHUD(GameState *game, Font font);
void DrawTelaMenu(GameState *game, Font font, float time);
void DrawTelaControles(GameState *game, Font font);
void UpdateTelaTutorial(GameState *game, Vector2 mouse);
void DrawTelaGameplay(GameState *game, Font font, bool drawHUD);
void DrawTelaPausa(GameState *game, Font font);
void DrawTelaGameOver(GameState *game, Font font);
void DrawTelaVitoria(GameState *game, Font font);

// Funções de atualização dos estados de hover e clique dos botões em cada tela
bool UpdateButtonsMenu(GameState *game, Vector2 mouse);
void UpdateButtonsControles(GameState *game, Vector2 mouse);
void UpdateButtonsPause(GameState *game, Vector2 mouse);
void UpdateButtonsGameOver(GameState *game, Vector2 mouse);
void UpdateButtonsVitoria(GameState *game, Vector2 mouse);
void UpdateButtonsSkins(GameState *game, Vector2 mouse);

// Tela de Tutorial (Seringa de Vacina)
void DrawTelaTutorial(GameState *game, Font font);

// Tela de Carregamento (Loading)
void DrawTelaLoading(GameState *game, Font font);

// Tela de Configurações
void DrawTelaSettings(GameState *game, Font font);
void UpdateButtonsSettings(GameState *game, Vector2 mouse, GameScreen backScreen);
// Funções de desenho e atualização das telas de seleção de save/load
void DrawTelaSaveSelect(GameState *game, Font font, Vector2 mouse, Texture2D slotTextures[3], bool slotTexturesLoaded[3]);
void DrawTelaLoadSelect(GameState *game, Font font, Vector2 mouse, Texture2D slotTextures[3], bool slotTexturesLoaded[3]);
int UpdateButtonsSaveSelect(GameState *game, Vector2 mouse, Texture2D slotTextures[3], bool slotTexturesLoaded[3]);
int UpdateButtonsLoadSelect(GameState *game, Vector2 mouse, Texture2D slotTextures[3], bool slotTexturesLoaded[3]);

// Tela de Quiz Educacional
void DrawTelaQuiz(GameState *game, Font font);
void UpdateTelaQuiz(GameState *game, Vector2 mouse);

// Tela de Upgrade do SUS
void DrawTelaUpgrade(GameState *game, Font font);
void UpdateTelaUpgrade(GameState *game, Vector2 mouse);

// Tela de Arsenal (detalhes de todas as armas)
void DrawTelaArsenal(GameState *game, Font font);
void UpdateTelaArsenal(GameState *game, Vector2 mouse);

// Tela dedicada de seleção de Skins (com preview)
void DrawTelaSkins(GameState *game, Font font);
void UpdateTelaSkins(GameState *game, Vector2 mouse);

// Tela do Modo Administrador / Dev (senha + configuração)
void DrawTelaAdmin(GameState *game, Font font);
void UpdateTelaAdmin(GameState *game, Vector2 mouse);

#endif // TELAS_H
