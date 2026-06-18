# 🦠 Disease's Doomsday

> **Jogo educativo de saúde pública desenvolvido para a disciplina de Projeto Integrador**
> Plataforma: Desktop (Windows10/11) · Linguagem: C + Raylib · Gênero: RPG de Ação 2D Top-Down

---

## 🎯 Proposta

**Disease's Doomsday** é um jogo de ação em arena com proposta **educacional de saúde pública**, desenvolvido como projeto acadêmico. O jogador assume o papel de um **Anticorpo** injetado na corrente sanguínea de um paciente infectado e precisa eliminar patógenos onda após onda, aprendendo conceitos de **vigilância epidemiológica, prevenção de doenças e funcionamento do sistema imune** ao longo da jornada.

O projeto une entretenimento e aprendizagem ativa, traduzindo conteúdos da área da saúde em mecânicas de jogo coerentes e didáticas.

---

## 🏥 Proposta Educacional

O jogo aborda, de forma lúdica e cientificamente embasada, as seguintes temáticas:

| Tema                     | Conteúdo Educativo                                                                  |
| ------------------------ | ----------------------------------------------------------------------------------- |
| **Superbactéria KPC**    | Resistência antimicrobiana, infecções hospitalares, uso responsável de antibióticos |
| **Dengue**               | Transmissão pelo _Aedes aegypti_, eliminação de criadouros, prevenção no DF/Brasil  |
| **Influenza**            | Vacinação, transmissão respiratória, medidas de proteção                            |
| **Pneumonia Bacteriana** | Agentes causadores, ambientes de risco, prevenção                                   |
| **Sistema Imune**        | Anticorpos, bacteriófagos, capsídeo viral, citocinas                                |

### Mecânicas com Intencionalidade Educativa

- 🧫 **Bacteriófagos** (arma ranged) → representam vírus que atacam bactérias, conceito real da microbiologia
- 💉 **Rifle de Vacina** → simboliza imunização como resposta antiviral
- 😷 **Máscara Hospitalar** (drop) → reduz dano recebido, reforça prevenção de transmissão
- 📏 **Distanciamento Social** (drop) → aura que repele inimigos, analogia direta às medidas sanitárias
- 🧬 **Capsídeo (escudo viral)** → mecânica de Mundo 2: só é possível eliminar o vírus após quebrar seu capsídeo proteico
- 🩺 **Quiz da Vigilância Epidemiológica** → perguntas entre ondas que ensinam cuidados reais e rendem **Pontos do SUS**

---

## 🗺️ Estrutura Narrativa

```
Menu Principal
   └─ Novo Jogo ─► Tutorial (Seringa de Vacina) ─► Cutscene de injeção
         └─► MUNDO 1 — Bactérias (5 ondas)
               ├─ Ondas 1–4: Inimigos bacterianos + Quiz + Melhorias do SUS
               └─ Onda 5: CHEFE — Superbactéria KPC (3 fases)
                     └─► Cutscene educativa de transição
                           └─► MUNDO 2 — Vírus (5 ondas)
                                 ├─ Ondas 1–4: Vírus com escudo de capsídeo
                                 └─ Onda 5: CHEFE viral com capsídeo reforçado
                                       └─► 🏆 VITÓRIA FINAL
```

---

## 🌍 Dois Mundos Temáticos

### 🦠 Mundo 1 — Bactérias

Ambientação: corrente sanguínea e ambiente hospitalar.

| Elemento    | Detalhe                                                                               |
| ----------- | ------------------------------------------------------------------------------------- |
| Inimigos    | Bactéria Corpo-a-corpo (cocos) · Bactéria à distância (bacilo atirador)               |
| Chefe       | **Superbactéria KPC** — 3 fases, escolta de lacaios, projéteis radiais                |
| Arma Melee  | **Espada-Seringa** — golpe 360° com knockback                                         |
| Arma Ranged | **Rifle de Bacteriófagos** — projétil especialmente eficaz vs. bactérias              |
| Drops       | **Máscara Hospitalar** (↓ dano recebido) · **Distanciamento Social** (aura repulsiva) |
| Órgão-alvo  | Pulmões (pneumonia) · Foco hospitalar (KPC)                                           |

### 🧬 Mundo 2 — Vírus

Ambientação: vias respiratórias e corrente sanguínea.

| Elemento    | Detalhe                                                                           |
| ----------- | --------------------------------------------------------------------------------- |
| Inimigos    | Vírus Corpo-a-corpo **com escudo de capsídeo** · Vírus à distância **com escudo** |
| Chefe       | Vírus Boss com capsídeo reforçado                                                 |
| Arma Melee  | **Escalpelizador Estático** — quebra capsídeo rapidamente                         |
| Arma Ranged | **Rifle de Vacina** — projétil eficaz vs. vírus                                   |
| Drops       | Compartilhados (ambos os mundos)                                                  |
| Órgão-alvo  | Pulmões (influenza) · Corrente sanguínea (dengue)                                 |

### 🤝 Itens Compartilhados (ambos os mundos)

| Item                                         | Efeito                                                           |
| -------------------------------------------- | ---------------------------------------------------------------- |
| **Desestabilizador de Ácidos Ribonucleicos** | Granada de área — ataca o RNA dos patógenos, pode reduzir escudo |
| **Citocina de Estabilização**                | Regenera vida do herói — analogia às citocinas do sistema imune  |

---

## 🕹️ Mecânicas de Jogo

### Controles

| Ação            | Tecla                           |
| --------------- | ------------------------------- |
| Mover           | `W` `A` `S` `D` ou setas        |
| Atacar          | **Clique esquerdo** ou `Espaço` |
| Trocar arma     | `1` `2` `3` `4`                 |
| Usar poção      | `E`                             |
| Avançar diálogo | `Q` ou `Espaço`                 |
| Pausar          | `Esc`                           |
| Salvar          | `F5`                            |
| Carregar        | `F9`                            |

### Progressão e RPG

- **Nível / XP**: eliminar patógenos concede XP; subir de nível aumenta vida, ataque e velocidade
- **Pontos do SUS**: acertar o quiz entre ondas rende pontos para gastar em melhorias permanentes:
  - `Vida Máxima +100` (50 pts)
  - `Velocidade +15%` (50 pts)
  - `Dano +10` (50 pts)

### Inimigos — IA e comportamentos

| Tipo                         | Comportamento                                                  |
| ---------------------------- | -------------------------------------------------------------- |
| SARS-CoV-2                   | Equilibrado, corpo a corpo                                     |
| Dengue (_Aedes aegypti_)     | Rápido, atirador, zigue-zague, foge quando o herói se aproxima |
| Superbactéria KPC            | Lento, muita vida, atirador pesado; **Chefe** na onda 5        |
| _Trypanosoma cruzi_          | Muito rápido, frágil, invoca mosquitos Aedes                   |
| _Mycobacterium tuberculosis_ | Atirador pesado, lento e resistente                            |

---

## 🏗️ Arquitetura do Projeto

```
Disease's_Doomsday/
├── Game/
│   ├── src/
│   │   ├── main.c                  # Janela, loop principal, música, letterbox
│   │   ├── logic/
│   │   │   ├── update_gameplay.c   # Núcleo: movimento, IA, colisões, ondas, save/load
│   │   │   └── fsm.c               # Máquina de estados das telas
│   │   ├── gameplay/
│   │   │   ├── input_controller.c  # Entrada de UI e botões/menus
│   │   │   └── spatial_grid.c      # Grid espacial para colisões otimizadas
│   │   ├── render/
│   │   │   └── render_gameplay.c   # HUD, mundo, inimigos, projéteis, tutorial
│   │   ├── entities/               # player.c/.h · enemy.c/.h · projectiles.c/.h
│   │   ├── systems/
│   │   │   ├── combat_system.c     # Dano ao jogador e efeitos
│   │   │   └── wave_manager.c      # Geração de ondas e do chefe (por Mundo)
│   │   ├── telas/                  # telas.c · tela_quiz.c · tela_upgrade.c
│   │   └── assets/
│   │       ├── asset_manager.c     # Fonte, música, SFX, shaders
│   │       └── sprite_manager.c    # Pipeline de sprites (PNG + fallback procedural)
│   ├── include/                    # game.h · gameplay.h · telas.h · sprite_manager.h
│   ├── Assets/
│   │   ├── Maps/                   # map_seringa.c (tutorial) · map_body.c (corpo humano)
│   │   ├── Sprites/                # Player/ · Enemies/ · Weapons/ · Items/ · UI/
│   │   └── @models/                # player_model.c · enemy_model.c · weapons_model.c
│   ├── Saves/                      # Saves (.txt) e screenshots de preview
│   ├── compilar.bat                # Build Windows
│   ├── Makefile                    # Build Linux/macOS
│   └── build.sh                    # Atalho Linux/macOS
├── README.md                       # Este documento
├── elicitacao.md                   # Documento de elicitação (5W2H, público-alvo, stakeholders)
├── COMO_RODAR.md                   # Guia detalhado de instalação
└── PROMPT_CLAUDE_CODE.md           # Prompt de expansão para IA de código
```

---

## 🔧 Destaques Técnicos

| Recurso             | Implementação                                                             |
| ------------------- | ------------------------------------------------------------------------- |
| **Renderização**    | Raylib — janela redimensionável com _letterbox_ 1280×720                  |
| **Arte**            | Procedural (fallback) com pipeline para receber sprites PNG               |
| **Câmera**          | Suave (lerp) com _screen shake_ em impactos                               |
| **Partículas**      | Pool de reuso eficiente                                                   |
| **Shaders**         | Efeito biológico + vinheta de vida baixa                                  |
| **Áudio**           | Música em crossfade + SFX contextuais                                     |
| **IA dos inimigos** | FSM (IDLE → AGGRO → ATTACK → HURT → DEATH) com flanqueamento              |
| **Save/Load**       | 3 slots (.txt) com screenshot de preview, escrita assíncrona em thread    |
| **HUD**             | Radar/biossensor, barra de chefe, hotbar de armas, rótulo de órgão/doença |
| **Grid Espacial**   | Colisões otimizadas por localidade                                        |

---

## 🚀 Como Compilar e Rodar

### Windows

Requer Raylib instalada em `C:\raylib` (instalador MinGW oficial).

```bat
cd Game
compilar.bat
rpg_game.exe
```

### Linux / macOS

```bash
cd Game
make        # gera ./rpg_game
./rpg_game
# ou:
./build.sh
```

> Guia detalhado de instalação: **[COMO_RODAR.md](COMO_RODAR.md)**

---

## 📋 Artefatos Documentados

| Documento                      | Descrição                                             |
| ------------------------------ | ----------------------------------------------------- |
| [README.md](README.md)         | Visão geral do projeto, arquitetura e instruções      |
| [elicitacao.md](elicitacao.md) | 5W2H, público-alvo, stakeholders, custo computacional |
| [COMO_RODAR.md](COMO_RODAR.md) | Guia completo de instalação e build                   |

---

## 👥 Equipe e Créditos

Projeto acadêmico desenvolvido em **C + Raylib** para a disciplina de **Projeto Integrador**, com enfoque em saúde pública do Distrito Federal.

Uso educacional — ao reutilizar ou distribuir, mantenha os créditos dos autores e da biblioteca [Raylib](https://www.raylib.com/).
