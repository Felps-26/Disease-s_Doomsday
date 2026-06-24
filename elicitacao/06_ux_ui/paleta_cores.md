# 6.2 Paleta de Cores (Temática Biológica)

> Valores extraídos diretamente das constantes `THEME_COLOR_*` e `COLOR_*` em `include/game.h`.

## Paleta de Tema (UI / Menus)

| Constante              | RGBA                        | Hex (aprox.) | Uso |
| ---------------------- | --------------------------- | ------------ | --- |
| `THEME_COLOR_MAIN`     | `{0, 229, 255, 255}`        | `#00E5FF`    | Bordas principais, destaques ciano |
| `THEME_COLOR_TEXT`     | `{80, 220, 120, 255}`       | `#50DC78`    | Texto biológico verde |
| `THEME_COLOR_BG_DARK`  | `{6, 18, 10, 255}`          | `#06120A`    | Fundo escuro teal (painéis) |
| `THEME_COLOR_BG_LIGHT` | `{10, 28, 18, 255}`         | `#0A1C12`    | Fundo claro teal (áreas internas) |
| `THEME_COLOR_PANEL`    | `{14, 10, 26, 255}`         | `#0E0A1A`    | Base de painéis e backgrounds |
| `THEME_COLOR_BORDER`   | `{84, 52, 148, 255}`        | `#543494`    | Bordas secundárias violeta |

## Paleta de Gameplay (Entidades)

| Token                  | Hex (aprox.) | Uso |
| ---------------------- | ------------ | --- |
| `COLOR_PLAYER`         | `#00BFFF`    | Anticorpo (herói) — ciano bioluminescente |
| `COLOR_ENEMY_BACTERIA` | `#FF4444`    | Inimigos bacterianos — vermelho |
| `COLOR_ENEMY_VIRUS`    | `#AA44FF`    | Inimigos virais — violeta |
| `COLOR_HEALTH`         | `#44FF44`    | Barra de vida — verde |
| `COLOR_SHIELD`         | `#4488FF`    | Escudo / capsídeo — azul |
| `COLOR_XP`             | `#FFD700`    | XP e pontos — dourado |
| `COLOR_BOSS`           | `#FF8800`    | Boss e alertas — laranja |
| `COLOR_BACKGROUND`     | `#0A0A1A`    | Fundo do mundo — azul-noite |
| `COLOR_UI`             | `#FFFFFF` + opacidade | Textos e ícones HUD |

## Skins de Arma (Paletas de cor)

| Skin (in-game) | Primary | Secondary | Uso |
| -------------- | ------- | --------- | --- |
| PADRAO | `{120, 200, 255}` (azul-imune) | `{235, 245, 255}` (branco-gelo) | Padrão |
| PLASMA | `{220, 70, 255}` (magenta) | `{0, 229, 255}` (ciano) | Cosmético |
| TOXICA | `{130, 220, 40}` (verde-ácido) | `{200, 255, 90}` (lima) | Cosmético |

## Skins de Jogador (Paletas de personagem)

| Skin (in-game) | ID | Visual |
| -------------- | -- | ------ |
| PADRAO | 0 | Anticorpo clássico ciano |
| MEDICA | 1 | Paleta clínica branca/azul |
| INFECTADA | 2 | Paleta roxa/verde degradada |
