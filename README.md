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
![Status](https://img.shields.io/badge/Status-v1.0%20Conclu%C3%ADdo-brightgreen?style=for-the-badge)

<br/>

> **Jogo educativo de saúde pública — desenvolvido como Projeto Integrador**
> Uma arena de ação 2D top-down onde cada inimigo é uma doença real, cada arma é um conceito científico, e cada vitória é uma aula de epidemiologia.
>
> **Palco: Distrito Federal — Educação em Saúde nas Escolas do DF**

</div>

---

## ⚠️ O CENÁRIO

```
╔══════════════════════════════════════════════════════════════════╗
║  ALERTA BIOLÓGICO — PROTOCOLO IMUNOLÓGICO ATIVADO               ║
║                                                                  ║
║  • KPC resistente a antibióticos detectada na corrente sanguínea ║
║  • Surto de Dengue ativo — vetor Aedes aegypti confirmado        ║
║  • Influenza em propagação aérea exponencial                     ║
║  • Sarampo mutante — alta resistência ao sistema imune           ║
║                                                                  ║
║  > INJETANDO ANTICORPO EM T-MENOS 3... 2... 1...                ║
╚══════════════════════════════════════════════════════════════════╝
```

O paciente está infectado. O sistema imunológico está sobrecarregado.
Você, um **anticorpo programado**, é a última linha de defesa.

---

## 🏫 POR QUE O DISTRITO FEDERAL?

**Disease's Doomsday** nasce de uma necessidade concreta: estudantes das escolas públicas e privadas do **Distrito Federal** convivem anualmente com surtos de dengue, chikungunya e zika — mas raramente recebem educação preventiva de forma envolvente. O DF é o palco central do projeto porque:

- Em anos epidêmicos, o DF figura entre as maiores incidências de dengue por 100 mil habitantes do Brasil — chegando a **decretar emergência em saúde pública em 2024**.
- Os hospitais de referência regional do DF convivem com **bactérias multirresistentes (KPC)**, tema do Mundo 1 do jogo.
- A faixa etária prioritária — estudantes do ensino médio da rede pública — consome jogos digitais como mídia principal, mas raramente encontra esse conteúdo de saúde em formato lúdico e acessível.

> _O DF vive a doença todo ano, mas a educação preventiva não acompanha esse ritmo. Disease's Doomsday ataca exatamente essa distância — dentro da sala de aula._

---

<div align="center">

## 🎮 O QUE É ESTE JOGO?

</div>

**Disease's Doomsday** é um jogo de ação em arena com proposta **educacional de saúde pública**, pensado para uso em **laboratórios de informática e salas de aula de Ciências e Biologia** do DF. O jogador assume o papel de um anticorpo injetado na corrente sanguínea de um paciente infectado — eliminando patógenos onda após onda enquanto aprende conceitos reais de **vigilância epidemiológica, prevenção de doenças e imunologia**.

O projeto une **entretenimento e aprendizagem ativa**, traduzindo conteúdos de saúde pública em mecânicas de jogo coerentes, didáticas e — acima de tudo — divertidas. O conteúdo é baseado em fontes do **SUS, OMS, SES-DF e Ministério da Saúde**.

---

## 🗂️ ONDE O JOGO SE ENCAIXA

**Disease's Doomsday** se posiciona na categoria dos ***serious games*** — jogos digitais desenvolvidos com objetivo primário de ensino, e não apenas de entretenimento. Dentro desse universo, ocupa um nicho específico: **jogo educativo de saúde pública para uso em ambiente escolar**, com foco no Ensino Médio do Distrito Federal.

### Como se diferencia de outras ferramentas de ensino

| Ferramenta | Limitação | O que o jogo oferece diferente |
| ---------- | --------- | ------------------------------ |
| Livro didático | Passivo, texto denso, sem feedback imediato | Aprendizagem ativa — o aluno age e vê as consequências |
| Vídeo educativo | Sem interação, fácil dispersão | Engajamento contínuo — cada onda exige atenção e decisão |
| Quiz online isolado | Sem contexto, sem narrativa | Quiz integrado à campanha — a pergunta faz sentido dentro do jogo |
| Simulação científica | Interface técnica, difícil para adolescentes | Interface de jogo familiar — controles simples, curva suave |
| Campanha de dengue (panfleto/palestra) | Alcance limitado, baixo engajamento jovem | Conteúdo da campanha dentro da mecânica — o aluno joga e aprende ao mesmo tempo |

### Onde é usado

```
Escola do DF
│
├── Laboratório de informática ──► Sessão de 20–40 min (individual ou dupla)
├── Aula de Ciências / Biologia ──► Ferramenta de fixação pós-conteúdo
├── Semana de saúde escolar ──────► Atividade temática (dengue, vacinação)
└── Uso doméstico ────────────────► Continuidade fora da escola (offline, gratuito)
```

### O que o jogo NÃO é

- Não é uma simulação científica precisa — é uma **metáfora lúdica** de conceitos reais
- Não substitui o professor — é uma **ferramenta de apoio pedagógico**
- Não exige internet ou assinatura — roda **offline, localmente, de graça**

---

## 🦠 OS INIMIGOS — Doenças Reais, Mecânicas Reais

### Mundo 1 — Bactérias

| Patógeno | Tipo | Comportamento em Jogo | Conceito Real |
| -------- | ---- | --------------------- | ------------- |
| 🟡 **Coco** | Inimigo Comum | Melee direto; perseguição com flanking | Bactérias de forma esférica (cocos) — infecções comuns |
| 🟠 **Bacilo** | Atirador | Ranged; projéteis ácidos com efeito de veneno | Bactérias de forma de bastão — infecções respiratórias |
| 🔴 **Superbactéria KPC** | CHEFE Onda 5 | 3 fases de IA; invoca minions; projéteis radiais | Resistência antimicrobiana — infecções hospitalares no DF |

### Mundo 2 — Vírus

| Patógeno | Tipo | Comportamento em Jogo | Conceito Real |
| -------- | ---- | --------------------- | ------------- |
| 🔵 **Rinovírus** | Swarm (bando) | Rápido e frágil; recua com HP baixo | Resfriado comum — transmissão por gotículas |
| 🟢 **Dengue** | Melee | Pressão corpo-a-corpo constante; capsídeo | Dengue endêmica no DF — vetor *Aedes aegypti* |
| 🟣 **Influenza** | Atirador | Ranged; projéteis virais com *charge timer* | Gripe — vacinação e transmissão respiratória |
| 🟤 **Sarampo Mutante** | Elite | Lento, resistente; aplica debuff de Lentidão | Sarampo — importância da vacinação em massa |
| 🔴 **Coronavírus** | CHEFE Onda 5 | 3 fases; Núcleos de Infecção; invulnerável até destruí-los | SARS-CoV-2 — protocolos de isolamento, vacinação |

> Todos os inimigos possuem **capsídeo viral** no Mundo 2 — escudo de pontos de vida que precisa ser destruído antes de atingir o HP.

---

## ⚔️ O ARSENAL — Ciência como Arma

> _Cada arma existe por um motivo científico. Não é ficção — é biologia._
> Cada slot **evolui após 30 abates** com aquele slot, desbloqueando uma forma mais poderosa.

```
┌─────────────────────────────────────────────────────────────────────────┐
│  SLOT 1 — Espada-Seringa  →  [30 abates]  →  Lâmina Bioelétrica        │
│  Base:  AoE 360°, raio 140 px, knockback — cooldown 0,24 s             │
│  Evol:  ×3 de dano contra capsídeo viral — counter do Mundo 2          │
├─────────────────────────────────────────────────────────────────────────┤
│  SLOT 2 — Rifle Contextual  →  [30 abates]  →  Rifle Vetorial Rep.     │
│  M1: Bacteriófago (vírus que mata bactéria) — +60% vs. bactérias       │
│  M2: Rifle de Vacina — +60% vs. vírus; troca automática de Mundo       │
│  Evol: Projétil duplica no 1º impacto                                  │
├─────────────────────────────────────────────────────────────────────────┤
│  SLOT 3 — Desestabilizador de RNA  →  [30 abates]  →  Lança-Minas RNA  │
│  Base:  Planta minas biológicas; Mouse 2 = detona todas                │
│  Evol:  Arremessa minas a 310 px; pavio 6 s (detona sozinha)           │
├─────────────────────────────────────────────────────────────────────────┤
│  SLOT 4 — BFG Imunológico  →  [30 abates]  →  BFG Imunológico Ômega   │
│  Base:  Canhão perfurante — atravessa todos os inimigos; cooldown 5 s  │
│  Evol:  Perfura + detona em explosão de área no fim do trajeto         │
└─────────────────────────────────────────────────────────────────────────┘
```

### Drops Especiais (Power-ups)

| Power-up | Conceito Real | Efeito |
| -------- | ------------- | ------ |
| 😷 Máscara Hospitalar | EPI hospitalar — PCIH/ANVISA | Reduz dano recebido em 40% |
| 📏 Distanciamento Social | Medida não-farmacológica do SUS | Aura repulsora de inimigos |
| 💉 Citocina de Estabilização | Citocinas anti-inflamatórias / Fiocruz | Cura +20 HP + regeneração por 6 s |
| 🧬 Desestabilizador de RNA | Interferência de RNA / terapia gênica | Explosão imediata em área |
| ✨ Orbe Supremo | Pico imunológico total | Todos os buffs por 7 s |
| 🛡️ Barreira de Plasma | Imunidade reforçada | Escudo + máscara por 12 s |

---

## 🗺️ MAPA DA CAMPANHA

```
  [INÍCIO]
     │
     ▼
  📋 Tutorial — Mapa da Seringa (1600×400 px)
     │  3 etapas guiadas: Movimento → Combate → Saída
     │
     ▼
  🎬 Dica educativa rotativa (tela de loading)
     │
     ▼
  ══════════════════════════════════════════
  🦠  MUNDO 1 — INFESTAÇÃO BACTERIANA
      Mapa: Corpo Humano (5657×5657 px)
  ══════════════════════════════════════════
     │
     ├── 🌊 Onda 1 ─── Cocos + Bacilos
     ├── 🌊 Onda 2 ─── + Miniboss KPC
     ├── 🌊 Onda 3 ─── Escalamento
     ├── 🌊 Onda 4 ─── Escalamento máximo
     │         │
     │    [Quiz entre ondas] + [Upgrades com Pontos do SUS]
     │
     └── 🌊 Onda 5 ─── 👑 CHEFE: Superbactéria KPC
                         Fase 1 → Fase 2 → Fase 3 (minions)
                              │
                              ▼
                   🎬 Cutscene de Transição Educativa
                              │
                              ▼
  ══════════════════════════════════════════
  🧬  MUNDO 2 — INVASÃO VIRAL
      Mapa: Corpo Humano (5657×5657 px)
  ══════════════════════════════════════════
     │
     ├── 🌊 Onda 1 ─── Rinovírus (Swarm)
     ├── 🌊 Onda 2 ─── + Dengue (Melee)
     ├── 🌊 Onda 3 ─── + Influenza (Ranged) + Sarampo Elite
     ├── 🌊 Onda 4 ─── Todos os tipos reforçados
     │         │
     │    [Quiz entre ondas] + [Upgrades] + [Desbloqueio de Evoluções]
     │
     └── 🌊 Onda 5 ─── 👑 CHEFE: Coronavírus
                         Fase 1: Núcleos de Infecção (boss invulnerável)
                         Fase 2: Ataque de alcance médio
                         Fase 3: Summon de minions a cada 6 s
                              │
                              ▼
                    🏆  VITÓRIA FINAL
                   "O paciente sobreviveu."
                   [Tela de estatísticas da campanha]
```

---

## 🎓 PROPOSTA EDUCACIONAL — Escolas do DF

O jogo é projetado para uso como **ferramenta pedagógica** nas aulas de Ciências e Biologia das escolas do Distrito Federal, alinhado com a BNCC e com as campanhas de saúde da SES-DF.

> **Para escolas:** gratuito · open-source · offline · sem cadastro · sem assinatura · roda em qualquer PC com OpenGL 3.3 e 512 MB de RAM

| Tema Abordado | Mecânica que Ensina | Alinhamento BNCC / SUS |
| ------------- | ------------------- | ---------------------- |
| **Dengue / Arboviroses no DF** | Inimigo Dengue (Melee com capsídeo) | EM13CNT207 — prevenção de doenças e promoção da saúde coletiva |
| **Resistência Antimicrobiana** | Boss KPC em 3 fases | EM13CNT303 — uso responsável de medicamentos; OMS top 10 ameaças |
| **Gripe / Influenza** | Inimigo Ranged com projéteis virais | PNI — Programa Nacional de Imunizações |
| **Coronavírus / SARS-CoV-2** | Boss Coronavírus (fases + isolamento) | EM13CNT207 — transmissão, isolamento e vacinação |
| **Sarampo / Vacinação** | Inimigo Elite com alta resistência | EM13CNT207 — importância da cobertura vacinal em massa |
| **Bacteriófagos** | Rifle de Bacteriófago (bônus vs. bactérias) | EM13CNT303 — biotecnologia e alternativas terapêuticas |
| **Sistema Imune** | Anticorpos, citocinas, capsídeo viral | EM13CNT301 — imunologia básica e biologia celular |
| **EPI / Distanciamento Social** | Power-ups Máscara e Distanciamento | ANVISA / SUS — medidas não-farmacológicas de controle epidemiológico |

### 🧠 Quiz da Vigilância Epidemiológica

Entre cada onda, o jogador responde perguntas sobre saúde pública. **48 perguntas** com embaralhamento Fisher-Yates (sem repetição das últimas 14). Acertar rende **Pontos do SUS** — moeda de melhorias permanentes:

```
✅ Resposta correta ──► +Pontos do SUS
                              │
             ┌────────────────┼─────────────────┐
             ▼                ▼                  ▼
      Vida Máxima +100   Velocidade +15%    Dano +10
          (50 pts)          (50 pts)        (50 pts)
```

> **Tópicos do banco de perguntas:** SUS, ESF, vacinação, dengue e arboviroses, resistência antimicrobiana, imunologia e microbiologia básica — todos com feedback educativo explicativo após a resposta.

---

## 🕹️ CONTROLES

| Ação | Tecla |
| ---- | ----- |
| Mover | `W` `A` `S` `D` ou `↑` `↓` `←` `→` |
| Atacar | **Clique Esquerdo** ou `Espaço` |
| Detonar minas | **Clique Direito** (Mouse 2) |
| Trocar Arma | `1` `2` `3` `4` |
| Usar Poção | `E` |
| Avançar Diálogo | `Q` ou `Espaço` |
| Pausar | `Esc` |
| Salvar | `F5` |
| Carregar | `F9` |

---

## 🏗️ ARQUITETURA DO PROJETO

```
Disease's_Doomsday/
├── Game/
│   ├── src/
│   │   ├── main.c                  # Janela, loop principal, música, letterbox
│   │   ├── logic/
│   │   │   ├── update_gameplay.c   # Núcleo: movimento, IA, colisões, ondas
│   │   │   └── fsm.c               # Máquina de estados (20 telas, enum GameScreen)
│   │   ├── gameplay/
│   │   │   ├── input_controller.c  # Entrada de UI e menus
│   │   │   └── spatial_grid.c      # Grid espacial para colisões otimizadas
│   │   ├── render/
│   │   │   └── render_gameplay.c   # HUD, mundo, inimigos, projéteis, tutorial
│   │   ├── entities/               # player.c · enemy.c · projectiles.c
│   │   ├── systems/
│   │   │   ├── combat_system.c     # Dano, I-frames, capsídeo, knockback, XP drops
│   │   │   └── wave_manager.c      # Geração determinística de ondas + chefes
│   │   ├── telas/                  # telas.c · tela_quiz.c · tela_upgrade.c
│   │   └── assets/
│   │       ├── asset_manager.c     # Fonte, música, SFX, shaders
│   │       └── sprite_manager.c    # Pipeline de sprites (PNG + fallback procedural)
│   ├── include/                    # game.h · gameplay.h · telas.h
│   ├── Assets/
│   │   ├── Maps/                   # map_seringa.c (tutorial) · map_body.c (campanha)
│   │   ├── Sprites/                # Player/ · Enemies/ · Weapons/ · Items/ · UI/
│   │   └── @models/                # player_model.c · enemy_model.c · weapons_model.c
│   ├── Saves/                      # Slots de save (.txt) + config.txt
│   ├── tests/                      # 5 testes em C puro
│   ├── compilar.bat                # Build Windows (MinGW)
│   ├── Makefile                    # Build Linux/macOS
│   └── build.sh                    # Atalho Linux/macOS
├── README.md
└── elicitacao/                     # Documentação completa do projeto
```

---

## 🔧 DESTAQUES TÉCNICOS

| Recurso | Implementação |
| ------- | ------------- |
| **Engine** | Raylib 5.x — janela redimensionável com _letterbox_ 1280×720 |
| **Supersampling** | Render em 2560×1440 (2×), downscale bilinear para 1280×720 |
| **Arte** | Pipeline procedural (render em C puro) + suporte a sprites PNG externos |
| **Câmera** | Suave (lerp) com _screen shake_ em impactos |
| **Partículas** | Pool de reuso eficiente — sem alocação dinâmica em runtime |
| **Shaders** | Efeito biológico (distorção orgânica) + vinheta de vida baixa |
| **Áudio** | Dual-track com crossfade suave (fade 12 s antes do fim da faixa) |
| **IA dos Inimigos** | FSM `IDLE → AGGRO → ATTACK → HURT → DEATH`; flanking, flee, dodging |
| **Colisão de Mapa** | Máscara de pixel gerada por `bake_collision_mask.py` (grade 452×452) |
| **Colisão de Entidades** | Grid espacial — O(1) por célula, sem brute-force |
| **Save/Load** | 3 slots `.txt` versionados (`AUDIO2`) + quicksave/load assíncrono |
| **HUD** | Radar/biossensor · barra de chefe · hotbar · rótulo de órgão/doença |
| **Testes** | 5 testes em C puro: rifle, ondas, mapa, núcleos, cosméticos |

---

## 🚀 COMO COMPILAR E RODAR

> **Pré-requisitos rápidos:** Git · GCC/MinGW (Windows) ou GCC (Linux/macOS) · Raylib 5.x

### 📥 Passo 0 — Baixar o Código-Fonte

Você precisa ter o **Git** instalado. Se não tiver, baixe em [git-scm.com](https://git-scm.com/).

**Opção A — via Git (recomendado):**

```bash
git clone https://github.com/Felps-26/Disease-s_Doomsday.git
cd "Disease-s_Doomsday/Game"
```

**Opção B — Download direto (sem Git):**

1. Acesse [github.com/Felps-26/Disease-s_Doomsday](https://github.com/Felps-26/Disease-s_Doomsday)
2. Clique em **Code → Download ZIP**
3. Extraia o arquivo e entre na pasta `Game/`

---

### 🪟 Windows

> Requer Raylib instalada em `C:\raylib` (instalador MinGW oficial)

**1. Instale o Raylib:**
Acesse [raylib.com](https://www.raylib.com/) → baixe `raylib_installer_vX.X.X.mingw.exe` → instale em `C:\raylib`

**2. Compile e execute** (dentro de `Game/`):

```bat
compilar.bat
rpg_game.exe
```

**3. (Opcional) Integração VS Code — `Game/.vscode/c_cpp_properties.json`:**

```json
{
  "configurations": [
    {
      "name": "Win32",
      "includePath": [
        "${workspaceFolder}/**",
        "C:/raylib/raylib/src",
        "C:/raylib/w64devkit/include"
      ],
      "compilerPath": "C:/raylib/w64devkit/bin/gcc.exe",
      "cStandard": "c11",
      "intelliSenseMode": "windows-gcc-x64"
    }
  ],
  "version": 4
}
```

---

### 🐧 Linux / 🍎 macOS

**1. Instale o Raylib:**

```bash
# Ubuntu/Debian
sudo apt install libraylib-dev

# macOS (Homebrew)
brew install raylib
```

**2. Compile e execute** (dentro de `Game/`):

```bash
make && ./rpg_game
# ou simplesmente:
./build.sh
```

> ⚠️ **Importante:** Execute **a partir da pasta `Game/`** — os caminhos de `Assets/` e `Saves/` são relativos ao executável.

---

## 📋 DOCUMENTAÇÃO

| Documento | Conteúdo |
| --------- | --------- |
| [`README.md`](README.md) | Visão geral, arquitetura, build e instalação |
| [`elicitacao/index.md`](elicitacao/index.md) | Índice completo da documentação de elicitação |
| [`elicitacao/01_identificacao_produto/`](elicitacao/01_identificacao_produto/identificacao.md) | Nome, tipo, stack, plataforma, tema |
| [`elicitacao/02_analise_5w2h/`](elicitacao/02_analise_5w2h/) | 5W2H — What · Why · Who · When · Where · How · How Much |
| [`elicitacao/04_requisitos/`](elicitacao/04_requisitos/) | Funcionais · Não Funcionais · Regras de Negócio |
| [`elicitacao/10_catalogo_mecanicas/`](elicitacao/10_catalogo_mecanicas/) | Inimigos · Armas · Power-ups |
| [`LICENSE`](LICENSE) | Licença MIT |

---

<div align="center">

## 👥 EQUIPE

**Projeto acadêmico desenvolvido em C + Raylib**
Disciplina: **Projeto Integrador** — Foco: Educação em Saúde Pública nas escolas do **Distrito Federal**

<br/>

| Integrante | GitHub |
| ---------- | ------ |
| **Filipi Ramos** | [@Felps-26](https://github.com/Felps-26) |
| **Thiago Vinicius Brito** | [@thivpb](https://github.com/thivpb) |
| **Mateus Gomes** | [@matts006](https://github.com/matts006) |

<br/>

_Uso educacional — ao reutilizar ou distribuir, mantenha os créditos dos autores_
_e da biblioteca [Raylib](https://www.raylib.com/)._

<br/>

---

<br/>

```
╔══════════════════════════════════════════════════════╗
║                                                      ║
║   [ O paciente está aguardando.                      ║
║     O anticorpo está a postos. ]                     ║
║                                                      ║
║              Boa sorte, Doutor.                      ║
║                                                      ║
╚══════════════════════════════════════════════════════╝
```

</div>
