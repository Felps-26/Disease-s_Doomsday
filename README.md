<div align="center">

<img src="assets/banner.png" alt="Disease's Doomsday Banner" width="100%"/>

<br/>

### 🧬 _Você é um anticorpo. Eles são a pandemia. Só um sobrevive._

<br/>

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)](LICENSE)
![Language: C](https://img.shields.io/badge/Language-C-blue?style=for-the-badge&logo=c)
![Engine: Raylib](https://img.shields.io/badge/Engine-Raylib-red?style=for-the-badge)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-green?style=for-the-badge)
![Genre](https://img.shields.io/badge/Genre-RPG%20Arena%202D-purple?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Jog%C3%A1vel%20%7C%20Campanha%20Completa-brightgreen?style=for-the-badge)

<br/>

> **Jogo educativo de saúde pública — desenvolvido como Projeto Integrador**
> Uma arena de ação 2D top-down onde cada inimigo é uma doença real, cada arma é um conceito científico, e cada vitória é uma aula de epidemiologia.

</div>

---

## ⚠️ O CENÁRIO

```
╔══════════════════════════════════════════════════════════════════╗
║  ALERTA BIOLÓGICO — PROTOCOLO IMUNOLÓGICO ATIVADO               ║
║                                                                  ║
║  • MUNDO 1: infestação bacteriana na corrente sanguínea          ║
║      └ cocos e bacilos atacando — Superbactéria KPC no comando   ║
║  • MUNDO 2: invasão viral com capsídeo reforçado                 ║
║      └ rinovírus, dengue, influenza e um chefe Coronavírus       ║
║                                                                  ║
║  > INJETANDO ANTICORPO EM T-MENOS 3... 2... 1...                ║
╚══════════════════════════════════════════════════════════════════╝
```

O paciente está infectado. O sistema imunológico está sobrecarregado.
Você, um **anticorpo programado**, é a última linha de defesa.

---

<div align="center">

## 🎮 O QUE É ESTE JOGO?

</div>

**Disease's Doomsday** é um jogo de ação em arena com proposta **educacional de saúde pública**. O jogador assume o papel de um anticorpo injetado na corrente sanguínea de um paciente infectado — eliminando patógenos onda após onda enquanto aprende conceitos reais de **vigilância epidemiológica, prevenção de doenças e imunologia**.

A campanha é dividida em **2 Mundos** (Bactérias → Vírus), cada um com **5 ondas** (a 5ª é sempre o chefe), conectados por uma **cutscene educativa**. O projeto une **entretenimento e aprendizagem ativa**, traduzindo conteúdos de saúde pública em mecânicas de jogo coerentes, didáticas e — acima de tudo — divertidas.

---

## 🦠 OS INIMIGOS — Doenças Reais, Mecânicas Reais

### 🌍 Mundo 1 — Infestação Bacteriana

| Patógeno                          | Papel           | Comportamento em Jogo                              | Conceito Real                                      |
| --------------------------------- | --------------- | ------------------------------------------------- | -------------------------------------------------- |
| 🟢 **Coco**                       | Corpo a corpo   | Persegue e cerca o jogador (flanqueamento)        | Bactéria esférica (pneumonia, infecções)           |
| 🟡 **Bacilo**                     | Atirador        | Mantém distância, dispara ácido (veneno)          | Bactéria em bastão; transmissão e contaminação     |
| 👑 **Superbactéria KPC**          | CHEFE Mundo 1   | 3 fases, escolta de lacaios, Núcleos de Infecção  | Resistência antimicrobiana, infecções hospitalares |

### 🧬 Mundo 2 — Invasão Viral _(todos com escudo de capsídeo)_

| Patógeno                          | Papel           | Comportamento em Jogo                              | Conceito Real                                      |
| --------------------------------- | --------------- | ------------------------------------------------- | -------------------------------------------------- |
| 🔴 **Rinovírus**                  | Enxame (swarm)  | Pequeno, veloz, frágil; recua com pouca vida      | Resfriado comum; transmissão respiratória          |
| 🦟 **Dengue** _(envelopado)_      | Corpo a corpo   | Pressão constante; usa o capsídeo como casca      | Arbovirose; vetor _Aedes aegypti_, criadouros      |
| 💨 **Influenza**                  | Atirador        | Dispara material viral (esporos) à distância      | Gripe; mutação anual, importância da vacinação     |
| 🟣 **Sarampo (mutante)**          | Elite           | Grande, lento, capsídeo reforçado, aplica lentidão | Paramyxovirus; vacinação e imunidade coletiva      |
| 👑 **Coronavírus**                | CHEFE Mundo 2   | 3 fases de IA, invoca lacaios, Núcleos de Infecção | Estrutura viral (capsídeo), respostas de contenção |

> 🧪 _Inimigos legados (SARS-CoV-2, Aedes, Trypanosoma cruzi, Tuberculose) permanecem no código por compatibilidade de saves._

---

## ⚔️ O ARSENAL — Ciência como Arma

> _Cada arma existe por um motivo científico. Não é ficção — é biologia._
> São **4 armas-base** (slots 1–4). Cada slot **evolui** depois de **30 abates com aquela arma**, virando uma versão mais forte (pressione o número do slot de novo para alternar).

```
┌──────────────────────────────────────────────────────────────────────────┐
│  SLOT 1 — CORPO A CORPO                                                   │
│  🔪 Espada-Seringa ............. Combo 360° (estocada + corte lateral)     │
│     └ evolui ► ⚡ Lâmina Bioelétrica .. quebra ESCUDOS (capsídeo) 3× mais  │
│                                                                            │
│  SLOT 2 — RIFLE (muda por Mundo)                                          │
│  🦠 Rifle de Bacteriófagos (M1) . +60% vs. BACTÉRIAS (vírus que as comem) │
│  💉 Rifle de Vacina (M2) ........ +60% vs. VÍRUS (imunização específica)   │
│     └ evolui ► 🟠 Rifle Vetorial Replicante .. duplica no 1º impacto       │
│                                                                            │
│  SLOT 3 — CONTROLE TÁTICO                                                 │
│  💣 Desestabilizador de RNA .... planta MINAS biológicas no chão           │
│     │                            (Mouse 2 detona todas de uma vez)         │
│     └ evolui ► 🟪 Lança-Minas de RNA .. arremessa minas à distância (6 s)  │
│                                                                            │
│  SLOT 4 — ARTILHARIA PESADA                                               │
│  🌟 BFG Imunológico ............ projétil que ATRAVESSA todos os inimigos  │
│     └ evolui ► ✴️ BFG Imunológico Ômega .. perfura E explode no fim        │
└──────────────────────────────────────────────────────────────────────────┘
```

### 💊 Power-ups (10 tipos — drops no mapa)

| Item | Efeito | Conceito |
| ---- | ------ | -------- |
| ❤️ **Recuperação de HP** | Cura imediata | Cuidado e recuperação |
| ⚡ **Boost de Velocidade** | Velocidade temporária | Resposta autonômica |
| 🛡️ **Escudo** | Absorve dano antes do HP | Barreira de anticorpos |
| ⚔️ **Boost de Ataque** | Dobra o dano causado | Resposta imune adaptativa |
| 😷 **Máscara Hospitalar** | Dano recebido −40% | EPI / controle de infecção |
| 📏 **Distanciamento Social** | Aura que repele inimigos | Medida não-farmacológica |
| 💥 **Desestabilizador de RNA** | Explosão em área imediata | Interferência de RNA |
| 💉 **Citocina de Estabilização** | Cura + regeneração contínua | Imunomodulação |
| 🌈 **Orbe Supremo** | Todos os sistemas reforçados (7 s) | Resposta imune total |
| 🔵 **Barreira de Plasma** | Escudo + máscara reforçados (12 s) | Imunidade reforçada |

---

## 🗺️ MAPA DA CAMPANHA

```
  [INÍCIO]
     │
     ▼
  📋 Tutorial — interior da Seringa de Vacina (3 etapas guiadas)
     │
     ▼
  🎬 Cutscene: "Injeção na corrente sanguínea"
     │
     ▼
  ══════════════════════════════
  🦠  MUNDO 1 — INFESTAÇÃO BACTERIANA
  ══════════════════════════════
     │
     ├── 🌊 Onda 1-4 ── Cocos/Bacilos + Miniboss + Quiz + Pontos do SUS
     │
     └── 🌊 Onda 5 ─── 👑 CHEFE: Superbactéria KPC
                              [FASE 1] → [FASE 2] → [FASE 3 c/ Núcleos]
                                               │
                                               ▼
                                    🎬 Cutscene Educativa (transição)
                                               │
                                               ▼
  ══════════════════════════════
  🧬  MUNDO 2 — INVASÃO VIRAL
  ══════════════════════════════
     │
     ├── 🌊 Onda 1-4 ── Vírus c/ Capsídeo + Miniboss + Quiz + Upgrades
     │
     └── 🌊 Onda 5 ─── 👑 CHEFE: Coronavírus (capsídeo + Núcleos)
                                               │
                                               ▼
                                     🏆  VITÓRIA FINAL
                                    "O paciente sobreviveu."
```

---

## 🎓 PROPOSTA EDUCACIONAL

O jogo aborda, de forma **lúdica e cientificamente embasada**, temas centrais de saúde pública:

| Tema                     | Conteúdo Ensinado                                                                   |
| ------------------------ | ----------------------------------------------------------------------------------- |
| **Superbactéria KPC**    | Resistência antimicrobiana, infecções hospitalares, uso responsável de antibióticos |
| **Dengue / Arboviroses** | Transmissão pelo _Aedes aegypti_, eliminação de criadouros, prevenção no DF/Brasil  |
| **Influenza**            | Vacinação anual, mutação viral, medidas de proteção individual e coletiva           |
| **Sarampo**              | Vacinação, imunidade de rebanho, importância da cobertura vacinal                   |
| **Sistema Imune**        | Anticorpos, bacteriófagos, capsídeo viral, papel das citocinas e vacinas            |
| **SUS & Vigilância**     | Princípios e diretrizes do SUS, notificação compulsória, ESF, controle social       |

### 🧠 Quiz da Vigilância Epidemiológica

Entre cada onda, o jogador responde perguntas de um banco de **48 perguntas** sobre saúde pública (com embaralhamento Fisher-Yates e histórico que evita repetir as 14 mais recentes). Cada resposta traz uma **explicação educativa**. Acertar rende **Pontos do SUS** — gastos em melhorias permanentes do personagem:

```
✅ Resposta correta ──► +Pontos do SUS
                              │
             ┌────────────────┼─────────────────┐
             ▼                ▼                  ▼
      Vida Máxima +100   Velocidade +15%    Dano +10
          (50 pts)          (50 pts)        (50 pts)
```

---

## 🕹️ CONTROLES

| Ação                 | Tecla                                       |
| -------------------- | ------------------------------------------- |
| Mover                | `W` `A` `S` `D` ou `↑` `↓` `←` `→`          |
| Atacar               | **Clique Esquerdo** ou `Espaço`             |
| Trocar / Evoluir Arma | `1` `2` `3` `4` _(repetir o nº alterna p/ a evolução)_ |
| Detonar Minas (RNA)  | **Clique Direito** (Mouse 2)                |
| Usar Poção           | `E`                                         |
| Avançar Diálogo      | `Q` ou `Espaço`                             |
| Pausar               | `Esc`                                       |
| Salvar rápido        | `F5`                                        |
| Carregar rápido      | `F9`                                        |

---

## ✨ DESTAQUES DE CONTEÚDO

| Sistema | Descrição |
| ------- | --------- |
| **2 Mundos** | Bactérias → Vírus, com cutscene educativa de transição |
| **3 Dificuldades** | Fácil / Médio / Difícil — escalam IA, HP, dano, velocidade e cadência do chefe |
| **Chefes multi-fase** | KPC e Coronavírus com 3 fases de IA e Núcleos de Infecção (invulneráveis até destruí-los) |
| **Escudo de Capsídeo** | Todo vírus tem escudo próprio que precisa ser quebrado antes do HP |
| **Evolução de Armas** | Cada slot evolui após 30 abates, virando uma versão mais poderosa |
| **Skins & Cosméticos** | 3 skins de personagem + 3 de arma + 8 slots de cosméticos (puramente visuais) |
| **Arsenal & Admin** | Tela de Arsenal (detalhes das armas) e Modo Administrador (debug, protegido por senha) |
| **Save/Load** | 3 slots manuais + auto-save, com metadados (nível, score, onda, data) e preview |

---

## 🏗️ ARQUITETURA DO PROJETO

```
Disease's_Doomsday/                # raiz do repositório
├── Disease's Doomsday/            # pasta do jogo (código + assets + build)
│   ├── src/
│   │   ├── main.c                  # Janela, loop principal, música, letterbox, supersampling
│   │   ├── logic/
│   │   │   ├── update_gameplay.c   # Núcleo: movimento, IA, colisões, ondas, dificuldade, save
│   │   │   └── fsm.c               # Máquina de estados das telas
│   │   ├── gameplay/
│   │   │   ├── input_controller.c  # Entrada de UI e menus
│   │   │   └── spatial_grid.c      # Grid espacial para colisões otimizadas
│   │   ├── render/
│   │   │   └── render_gameplay.c   # HUD, mundo, inimigos, projéteis, tutorial
│   │   ├── entities/               # player · enemy · projectiles (.c/.h)
│   │   ├── systems/
│   │   │   ├── combat_system.c     # Dano ao jogador, capsídeo, knockback e efeitos
│   │   │   └── wave_manager.c      # Geração de ondas e do chefe (por Mundo)
│   │   ├── telas/                  # telas · tela_quiz · tela_upgrade · tela_arsenal · tela_skins · tela_admin
│   │   └── assets/
│   │       ├── asset_manager.c     # Fonte, música, SFX, shaders
│   │       └── sprite_manager.c    # Pipeline de sprites (PNG + fallback procedural)
│   ├── include/                    # game.h · gameplay.h · telas.h · spatial_grid.h · ...
│   ├── Assets/
│   │   ├── Maps/                   # map_seringa (tutorial) · map_body (corpo = arena)
│   │   ├── Sprites/                # Player/ · Enemies/ · Weapons/ · Items/ · UI/ (PNG opcional)
│   │   ├── @models/                # player_model · enemy_model · weapons_model · doctor_model · cosmetics
│   │   ├── Musica/ · SFX/          # trilha (crossfade) e efeitos contextuais
│   │   └── Shaders/                # biological.fs · low_hp_vignette.fs
│   ├── Saves/                      # slots .txt + auto_save + config.txt + screenshot de preview
│   ├── tests/                      # test_*.c — suíte em C puro, sem framework externo
│   ├── tools/                      # utilitários (bake_collision_mask, previews, captures)
│   ├── compilar.bat                # Build Windows
│   ├── Makefile                    # Build Linux/macOS
│   └── build.sh                    # Atalho Linux/macOS
├── README.md
├── elicitacao/                     # Documento de Elicitação (5W2H, requisitos, UX, custo, catálogo...)
├── assets/                         # banner do repositório
└── LICENSE
```

---

## 🔧 DESTAQUES TÉCNICOS

| Recurso             | Implementação                                                          |
| ------------------- | ---------------------------------------------------------------------- |
| **Engine**          | Raylib (C11) — janela com _letterbox_ a partir de 1280×720             |
| **Supersampling**   | Render do mundo em 2× (2560×1440) reduzido no blit (SSAA) — mais nitidez |
| **Arte**            | Pipeline procedural com suporte a sprites PNG externos (fallback automático) |
| **Câmera**          | Suave (lerp) com _screen shake_ em impactos                            |
| **Partículas**      | Pool de reuso eficiente (sem alocação dinâmica em runtime)             |
| **Shaders**         | Efeito biológico (distorção orgânica) + vinheta de vida baixa          |
| **Áudio**           | Música dual-track em crossfade + SFX contextuais por evento; volumes independentes |
| **IA dos Inimigos** | 5 comportamentos (MELEE/RANGED/SWARM/ELITE/BOSS) com flanqueamento, esquiva e memória de aggro |
| **Save/Load**       | 3 slots manuais + auto-save `.txt` com screenshot de preview, escrita assíncrona |
| **HUD**             | Radar/biossensor · barra de chefe · hotbar de armas · rótulo de órgão/doença |
| **Colisões**        | Máscara de pixel do corpo + grid espacial — sem brute-force            |

---

## 🚀 COMO COMPILAR E RODAR

> ⚠️ **Importante:** todos os comandos devem ser executados **dentro da pasta `Disease's Doomsday/`** — os caminhos de `Assets/` e `Saves/` são relativos ao executável.

### 🪟 Windows

> Requer Raylib instalada em `C:\raylib` (instalador MinGW oficial)

**1. Instale o Raylib:**
Acesse [raylib.com](https://www.raylib.com/) → baixe `raylib_installer_vX.X.X.mingw.exe` → instale em `C:\raylib`

**2. Compile e execute** (dentro de `Disease's Doomsday/`):

```bat
compilar.bat
rpg_game.exe
```

> 💡 O `compilar.bat` define internamente os caminhos do GCC (`C:\raylib\w64devkit\bin\gcc.exe`) e do raylib (`C:\raylib\raylib\src`).

---

### 🐧 Linux / 🍎 macOS

**1. Instale o Raylib:**

```bash
# Ubuntu/Debian
sudo apt install libraylib-dev

# macOS (Homebrew)
brew install raylib
```

**2. Compile e execute** (dentro de `Disease's Doomsday/`):

```bash
make && ./rpg_game
# ou simplesmente:
./build.sh
```

> 🧩 Ao adicionar um novo `.c`, lembre de atualizar **`compilar.bat`** (variável `SOURCES`) **e** o **`Makefile`** (variável `SRCS`).

---

## 📋 DOCUMENTAÇÃO

| Documento                                    | Conteúdo                                                        |
| -------------------------------------------- | -------------------------------------------------------------- |
| [`README.md`](README.md)                     | Visão geral, arquitetura, build e instalação                    |
| [`elicitacao/`](elicitacao/index.md)         | 5W2H, requisitos, stakeholders, UX/UI, custo, segurança, catálogo de mecânicas |
| [`LICENSE`](LICENSE)                         | Licença MIT                                                     |

---

<div align="center">

## 👥 EQUIPE

**Projeto acadêmico desenvolvido em C + Raylib**
Disciplina: **Projeto Integrador** — com enfoque em saúde pública do Distrito Federal

<br/>

| [<img src="https://avatars.githubusercontent.com/u/188877921?v=4" width="90" alt="Filipi Ramos"/>](https://github.com/Felps-26) | [<img src="https://avatars.githubusercontent.com/u/273160477?v=4" width="90" alt="Thiago Vinícius Brito"/>](https://github.com/thivpb) | [<img src="https://avatars.githubusercontent.com/u/282407596?v=4" width="90" alt="Mateus Gomes"/>](https://github.com/matts006) |
| :---: | :---: | :---: |
| **Filipi Ramos** | **Thiago Vinícius Brito** | **Mateus Gomes** |
| [@Felps-26](https://github.com/Felps-26) | [@thivpb](https://github.com/thivpb) | [@matts006](https://github.com/matts006) |

<br/>

_Uso educacional — ao reutilizar ou distribuir, mantenha os créditos dos autores_
_e da biblioteca [Raylib](https://www.raylib.com/)._

<br/>

---

```
 ┌─ BRIEFING DA MISSÃO ──────────────────────────────────────
 │
 │   > Patógenos detectados na corrente sanguínea.
 │   > Anticorpo injetado ......................... OK
 │   > Sistema imune .............................. ONLINE
 │
 │   ❯  O paciente está aguardando. O anticorpo está a postos.
 │   ❯  Boa sorte, Doutor.
 │
 └────────────────────────────────────────────────────────────
```

</div>
