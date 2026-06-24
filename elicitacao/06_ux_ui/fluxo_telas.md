# 6.4 Fluxo de Telas (User Flow)

> O enum `GameScreen` define **20 estados** de tela (incl. `SCREEN_STAGE_COMPLETE` e `SCREEN_STAGE_PROLOGUE`,
> usados nas pausas curtas entre fim de onda → quiz → próxima onda). As principais estão documentadas abaixo;
> a FSM vive em `src/logic/fsm.c`.

## Diagrama de Transições

```
[SCREEN_MENU]
    ├── Nova Partida ──► [SCREEN_DIFFICULTY_SELECT]
    │                         └── Confirmar ──► [SCREEN_LOADING] ──► [SCREEN_TUTORIAL]
    ├── Carregar ──► [SCREEN_LOAD_SELECT] ──► [SCREEN_LOADING] ──► [SCREEN_GAMEPLAY]
    ├── Arsenal ──► [SCREEN_ARSENAL]
    ├── Skins ──► [SCREEN_SKINS]
    ├── Config ──► [SCREEN_SETTINGS]
    └── Sair ──► encerra o processo

[SCREEN_TUTORIAL]
    └── Completa 3 etapas ──► [SCREEN_LOADING] ──► [SCREEN_GAMEPLAY]

[SCREEN_GAMEPLAY]
    ├── ESC ──► [SCREEN_PAUSE]
    ├── HP ≤ 0 ──► [SCREEN_GAMEOVER] ──► [SCREEN_MENU]
    ├── Boss Mundo 1 derrotado ──► [SCREEN_WORLD_TRANSITION] ──► [SCREEN_LOADING] ──► [SCREEN_GAMEPLAY (Mundo 2)]
    ├── Boss Mundo 2 derrotado ──► [SCREEN_VICTORY] ──► [SCREEN_MENU]
    └── Entre ondas ──► [SCREEN_QUIZ] ──► [SCREEN_UPGRADE]

[SCREEN_PAUSE]
    ├── Retomar ──► [SCREEN_GAMEPLAY]
    ├── Salvar ──► [SCREEN_SAVE_SELECT] ──► [SCREEN_PAUSE]
    ├── Carregar ──► [SCREEN_LOAD_SELECT] ──► [SCREEN_LOADING] ──► [SCREEN_GAMEPLAY]
    ├── Config ──► [SCREEN_SETTINGS] ──► [SCREEN_PAUSE]
    ├── Quiz manual ──► [SCREEN_QUIZ] ──► [SCREEN_PAUSE]
    ├── Upgrade ──► [SCREEN_UPGRADE] ──► [SCREEN_PAUSE]
    └── Menu Principal ──► [SCREEN_MENU]

[SCREEN_WORLD_TRANSITION]
    └── Cutscene completa ──► [SCREEN_LOADING] ──► [SCREEN_GAMEPLAY]

[SCREEN_LOADING]
    └── LoadTarget resolve destino: TUTORIAL / GAMEPLAY / MENU / GAMEOVER / VICTORY
```

## Tabela de Telas

| ID (enum GameScreen)         | Descrição |
| ---------------------------- | --------- |
| `SCREEN_MENU`                | Menu principal com opções de jogo |
| `SCREEN_GAMEPLAY`            | Loop de jogo principal |
| `SCREEN_PAUSE`               | Pausa com submenu completo |
| `SCREEN_TUTORIAL`            | Fase da seringa — 3 etapas guiadas |
| `SCREEN_QUIZ`                | Quiz educativo entre ondas |
| `SCREEN_UPGRADE`             | Melhorias com Pontos do SUS |
| `SCREEN_ARSENAL`             | Exibição e detalhes das 4 armas-base e suas 4 evoluções |
| `SCREEN_SKINS`               | Preview ao vivo de skins e cosméticos |
| `SCREEN_SETTINGS`            | Volume (música + SFX), acessibilidade |
| `SCREEN_ADMIN`               | Painel de debug / admin |
| `SCREEN_WORLD_TRANSITION`    | Cutscene narrativa entre Mundo 1 e 2 |
| `SCREEN_DIFFICULTY_SELECT`   | Seleção de dificuldade (3 cards visuais) |
| `SCREEN_SAVE_SELECT`         | Seleção de slot para salvar |
| `SCREEN_LOAD_SELECT`         | Seleção de slot para carregar |
| `SCREEN_LOADING`             | Loading com dica educativa rotativa |
| `SCREEN_GAMEOVER`            | Derrota — opção de recomeçar ou menu |
| `SCREEN_VICTORY`             | Vitória — estatísticas da campanha |
| `SCREEN_STAGE_COMPLETE`      | Pausa curta entre o fim da onda e o quiz |
| `SCREEN_STAGE_PROLOGUE`      | Prólogo breve antes da próxima onda |

## Animações de Transição

- `screenAnim`: acumulador de tempo (reinicia a 0 em cada mudança de tela)
- `UIEase(t)`: função cúbica ease-out para fade e slide de entrada
- Tela de Loading exibe dica educativa rotativa (`GetLoadingTipText`)
- Duração mínima de loading: ~2 s (evita flash de tela)
