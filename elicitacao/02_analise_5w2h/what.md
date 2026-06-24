# 2.1 What — O quê?

**Disease's Doomsday** é um jogo de ação 2D _top-down_ desenvolvido em C + Raylib. O jogador controla um **Anticorpo** injetado na corrente sanguínea de um paciente infectado e deve eliminar patógenos onda após onda, aprendendo sobre doenças reais, formas de transmissão e medidas de prevenção do SUS.

---

## Funcionalidades Implementadas

### Campanha

| Item | Detalhe |
| ---- | ------- |
| Mundos | 2 — Bactérias e Vírus, com transição narrativa entre eles |
| Ondas por mundo | 5 (onda 5 = sempre chefe) |
| Total de ondas | 10 ondas de combate + 2 chefes principais |
| Mapa do Mundo 1-2 | 5657 × 5657 px — mapa do corpo humano com máscara de colisão por pixel |
| Tutorial | Mapa separado (seringa, 1600 × 400 px) com 3 etapas: movimento, combate, saída |

### Inimigos

| Categoria | Tipos | Exemplos |
| --------- | ----- | -------- |
| Bactérias | Melee, Ranged, Miniboss (KPC), Boss (KPC) | Coco, Bacilo, Superbactéria KPC |
| Vírus | Swarm, Melee, Ranged, Elite, Boss | Rinovirus, Dengue, Influenza, Sarampo mutante, Coronavírus |
| Total de tipos | 12 tipos com IDs 0–11 | |

### Armas (4 slots-base + 4 evoluções = 8 formas)

> Cada slot evolui após **30 abates com aquele slot** (`WEAPON_EVOLVE_KILLS = 30`).
> Pressionar o número do slot alterna entre a base e a evolução desbloqueada.

| Slot | Base | Mecânica base | Evolução (30 abates) |
| ---- | ---- | ------------- | -------------------- |
| 1 | Espada-Seringa | AoE 360°, raio 140 px, cooldown 0.24 s | **Lâmina Bioelétrica** — ×3 vs. escudo de capsídeo |
| 2 | Rifle contextual | Bacteriófago (M1) / Vacina (M2); +60% vs. tipo correto; cooldown 0.28 s | **Rifle Vetorial Replicante** — duplica no 1º impacto |
| 3 | Desestabilizador de RNA | **Planta minas** biológicas; Mouse 2 detona todas; cooldown 0.75 s | **Lança-Minas de RNA** — arremessa minas (pavio 6 s) |
| 4 | BFG Imunológico | Projétil perfurante (atravessa todos); cooldown 5 s | **BFG Imunológico Ômega** — perfura + explode no fim |

### Sistema de Quiz Educativo

- **48 perguntas** sobre SUS, vigilância epidemiológica, vacinação, dengue, resistência antimicrobiana e microbiologia
- Aparece entre ondas, com explicação educativa após cada resposta
- Embaralhamento por Fisher-Yates; histórico de 14 perguntas recentes evita repetição

### Progressão

| Recurso | Descrição |
| ------- | --------- |
| XP / Nível | Ganho por kills; sobe atributos do anticorpo |
| Pontos do SUS | Moeda de upgrades mid-run (HP máx., ataque, velocidade) |
| Score | Acumulado por toda a campanha |
| Desbloqueáveis | 4 evoluções de arma (30 abates/slot), skins (3 jogador + 3 arma), cosméticos (8 slots) |

### Power-ups (10 tipos)

`HP_RECOVERY` · `SPEED_BOOST` · `SHIELD` · `ATTACK_BOOST` · `POWERUP_MASK` (dano −40%) · `POWERUP_DISTANCING` (aura repulsora) · `POWERUP_RNA_GRENADE` (explosão em área) · `POWERUP_CYTOKINE` (cura + regeneração) · `POWERUP_SUPREME_ORB` (todos os buffs, 7 s) · `POWERUP_BARRIER` (escudo + máscara, 12 s)

### Save / Load

- 3 slots com metadados (nome, nível, score, onda, data)
- Quicksave F5 / Quickload F9
- Formato `.txt` com header versionado (`AUDIO2`)

### Outras telas

Arsenal · Skins · Cosméticos · Configurações (volume, skin) · Seleção de dificuldade (Fácil / Médio / Difícil) · Tela de vitória · Game Over · Transição de mundo (cutscene)
