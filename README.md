# Disease's Doomsday — RPG de Ação em C com Raylib

Jogo 2D top-down de sobrevivência em arena, desenvolvido em **C** com a biblioteca **Raylib**. É um projeto acadêmico com proposta **educacional de saúde pública (DF)**: o jogador controla um **Anticorpo** injetado na corrente sanguínea de um paciente infectado e precisa eliminar patógenos onda após onda, aprendendo conceitos de vigilância epidemiológica no caminho.

> O título da janela é **"Disease's Doomsday"**. Este documento substitui a documentação antiga (que descrevia um protótipo anterior chamado "Void Venture") e reflete o estado atual e real do projeto modular.

---

## Premissa e proposta

Você é um Anticorpo convocado para salvar um paciente. A jornada começa **dentro de uma Seringa de Vacina** (tutorial), de onde você é injetado no organismo e enfrenta cinco **ondas de infecção**, terminando em um confronto com a **Superbactéria KPC** (chefe). Entre as ondas, um **quiz da Vigilância Epidemiológica** reforça o conteúdo educativo e rende **Pontos do SUS**, gastos em melhorias do personagem.

A essência é um **RPG/ação** com combate em tempo real, progressão (nível e upgrades), inimigos variados, armas, power-ups, efeitos visuais e sonoros, e telas completas de jogo.

---

## Fluxo do jogo

```
Menu Principal
   └─ Novo Jogo ─► Tutorial (Seringa de Vacina, 3 passos) ─► Cutscene de injeção
         └─► Organismo (Gameplay)
               ├─ Onda 1 ─► Quiz ─► Melhorias do SUS ─► Onda 2 ...
               ├─ ...
               └─ Onda 5 (CHEFE: Superbactéria KPC) ─► Vitória
   └─ Carregar Jogo (3 slots com preview)
   └─ Controles / Configurações
```

A morte do Anticorpo leva à tela de **Game Over** (Tentar Novamente pula o tutorial). Vencer a onda 5 leva à tela de **Vitória**.

---

## Controles

| Ação | Tecla / Botão |
|---|---|
| Mover | `W` `A` `S` `D` ou as **setas** |
| Atacar | **Botão esquerdo do mouse** ou `Espaço` |
| Trocar de arma | `1` `2` `3` `4` |
| Usar poção de vida | `E` |
| Avançar diálogo (tutorial) | `Q` ou `Espaço` |
| Pausar / voltar | `Esc` |
| Salvar rápido (slot 1) | `F5` |
| Carregar rápido (slot 1) | `F9` |

As quatro armas estão disponíveis desde o início (não há bloqueio de armas).

---

## Armas

| # | Arma | Tipo | Característica | Cooldown |
|---|---|---|---|---|
| 1 | Lâmina Imunológica | Corpo a corpo | Golpe em **360°** ao redor, com empurrão (knockback) | 0,22 s |
| 2 | Fuzil Célula-T | Projétil rápido | Tiro reto veloz, dano por acerto | 0,15 s |
| 3 | Granada Macrófago | Explosivo | Dano em área **+ veneno (DoT)** nos sobreviventes — ideal contra alvos tanques | 1,5 s |
| 4 | Vacina BFG | Projétil pesado | Projétil **perfurante** de dano alto | 5,0 s |

---

## Patógenos (inimigos)

| Tipo | Patógeno | Comportamento |
|---|---|---|
| 0 | SARS-CoV-2 | Equilibrado, corpo a corpo |
| 1 | Vírus da Dengue (Aedes aegypti) | Rápido, atirador, movimento em zigue-zague e foge se você se aproxima |
| 2 | Superbactéria KPC | Lento, muita vida, atirador pesado (elite a partir da onda 3; **chefe** na onda final) |
| 3 | Trypanosoma cruzi (Chagas) | Muito rápido, frágil, corpo a corpo; **invoca** mosquitos Aedes |
| 4 | Mycobacterium tuberculosis | Atirador pesado, lento e resistente |

Os inimigos usam uma máquina de estados: `IDLE` (parado/patrulha), `AGGRO` (perseguindo), `ATTACK` (atacando), `HURT` (atordoado), `DEATH` (morrendo).

---

## Progressão

- **Nível / XP:** derrotar patógenos concede XP. Ao subir de nível, aumentam vida máxima, ataque e velocidade, e a vida é restaurada.
- **Pontos do SUS:** acertar o quiz entre ondas dá pontos para gastar na tela **Melhorias do SUS**:
  - Vida Máxima `+100` (50 pts)
  - Velocidade `+15%` (50 pts)
  - Dano `+10` (50 pts)

## Power-ups (no mapa e drops)

| Power-up | Efeito |
|---|---|
| Recuperação de HP | Restaura vida |
| Speed Boost | Velocidade temporária |
| Shield | Absorve dano por alguns segundos |
| Attack Boost | Dobra o dano temporariamente |

---

## Destaques técnicos

- Sistema de **partículas com pool** (reuso eficiente de slots).
- **Câmera suave** (lerp) com **screen shake** em impactos e na cutscene de injeção.
- **Números de dano flutuantes** e feedback de combate.
- **Radar/Biossensor** no HUD e **barra de vida fixa do chefe** no topo.
- **Shaders**: efeito biológico em tela cheia e vinheta de vida baixa.
- **Áudio** com música em **crossfade** e efeitos de ataque, dano e coleta.
- **Save/Load** em 3 slots (arquivos `.txt`) com **imagem de preview**; salvamento assíncrono em thread.
- **Janela redimensionável** com escalonamento *letterbox* (resolução virtual 1280×720).

---

## Estrutura do projeto

```
Game/
├── src/
│   ├── main.c                 # Janela, loop principal, música, letterbox
│   ├── logic/
│   │   ├── update_gameplay.c  # Núcleo: movimento, IA, colisões, ondas, save/load, tutorial
│   │   └── fsm.c              # Máquina de estados das telas (update + draw)
│   ├── gameplay/
│   │   ├── input_controller.c # Botões/menus e entrada de UI
│   │   └── spatial_grid.c     # Grid espacial para colisões otimizadas
│   ├── render/
│   │   └── render_gameplay.c  # HUD, mundo, inimigos, projéteis, tutorial
│   ├── entities/              # player, enemy, projectiles (.c/.h)
│   ├── systems/
│   │   ├── combat_system.c    # Dano ao jogador e efeitos
│   │   └── wave_manager.c     # Geração de ondas e do chefe
│   ├── telas/                 # telas.c, tela_quiz.c, tela_upgrade.c
│   └── assets/asset_manager.c # Carregamento de fonte, música, SFX e shaders
├── include/                   # Cabeçalhos globais (game.h, gameplay.h, telas.h, ...)
├── Assets/                    # Fonte, música, shaders, modelos (@models) e mapa da seringa
├── Saves/                     # Saves e screenshots de preview
├── compilar.bat               # Build no Windows
├── Makefile                   # Build no Linux/macOS
└── build.sh                   # Atalho de build no Linux/macOS
```

> Observação: `src/telas/telas_common.c`, `debug_patch.c` e `split.py` **não** fazem parte do build (são utilitários/legado) e podem ser ignorados.

---

## Como compilar e rodar

### Windows
Requer Raylib instalada em `C:\raylib` (instalador MinGW oficial). No diretório `Game/`:

```bat
compilar.bat
rpg_game.exe
```

### Linux / macOS
Requer a Raylib instalada e detectável via `pkg-config` (ex.: `apt install libraylib-dev`, `brew install raylib`, ou compilada do código-fonte). No diretório `Game/`:

```bash
make            # gera ./rpg_game
./rpg_game

# ou simplesmente:
./build.sh
```

O guia detalhado de instalação está em **COMO_RODAR.md**.

---

## Créditos e licença

Projeto acadêmico em **C + Raylib** para estudo de modularização, ponteiros, estruturas, manipulação de arquivos e desenvolvimento de jogos 2D. Uso educacional — ao reutilizar ou distribuir, mantenha os créditos dos autores e da biblioteca Raylib.
