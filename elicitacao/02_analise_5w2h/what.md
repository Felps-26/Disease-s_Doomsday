# 2.1 What — O quê?

**Disease's Doomsday** é um jogo de ação 2D _top-down_ desenvolvido em C + Raylib. O jogador controla um **Anticorpo** injetado na corrente sanguínea de um paciente infectado e deve eliminar patógenos onda após onda, aprendendo sobre doenças reais, formas de transmissão e medidas de prevenção do SUS.

Concebido como **ferramenta pedagógica para escolas do Distrito Federal**, o jogo aborda doenças que fazem parte da realidade epidemiológica local — especialmente a dengue endêmica e a resistência antimicrobiana hospitalar — em uma experiência lúdica e didática.

---

## Funcionalidades Implementadas (v1.0 Concluído)

### Campanha

| Item | Detalhe |
| ---- | ------- |
| Mundos | 2 — Bactérias e Vírus, com cutscene educativa de transição |
| Ondas por mundo | 5 (onda 5 = sempre chefe) |
| Total de ondas | 10 ondas de combate + 2 chefes principais |
| Mapa | 5657 × 5657 px — corpo humano com máscara de colisão por pixel |
| Tutorial | Mapa separado (seringa, 1600 × 400 px) com 3 etapas guiadas: Movimento → Combate → Saída |

### Inimigos (7 tipos ativos + 4 legados)

| Mundo | Tipo | Nome | Comportamento | Conceito Real |
| ----- | ---- | ---- | ------------- | ------------- |
| 1 | Melee | Coco | Perseguição direta com flanking | Bactéria esférica |
| 1 | Ranged | Bacilo | Projéteis ácidos (veneno 3 s) | Bactéria de bastão |
| 1 | Boss | Superbactéria KPC | 3 fases; minions; projéteis radiais | KPC resistente a antibióticos |
| 2 | Swarm | Rinovírus | Bando frágil; recua com HP baixo | Resfriado comum |
| 2 | Melee | Dengue | Pressão corpo-a-corpo; capsídeo | Dengue endêmica no DF |
| 2 | Ranged | Influenza | Projéteis virais com chargeTimer | Gripe / H1N1 |
| 2 | Elite | Sarampo Mutante | Lento, resistente; debuff de Lentidão | Sarampo — importância vacinal |
| 2 | Boss | Coronavírus | 3 fases; Núcleos de Infecção; minions | SARS-CoV-2 |

> Todos os vírus (Mundo 2) possuem **capsídeo** — escudo de pontos de vida que precisa ser destruído antes do HP.

### Armas (4 slots-base + 4 evoluções = 8 formas)

> Cada slot evolui após **30 abates com aquele slot** (`WEAPON_EVOLVE_KILLS = 30`).

| Slot | Base | Mecânica base | Evolução (30 abates) | Efeito da evolução |
| ---- | ---- | ------------- | -------------------- | ------------------ |
| 1 | Espada-Seringa | AoE 360°, raio 140 px, cooldown 0,24 s | Lâmina Bioelétrica | ×3 dano vs. capsídeo |
| 2 | Rifle contextual | Bacteriófago (M1) / Vacina (M2); +60% vs. tipo correto; cooldown 0,28 s | Rifle Vetorial Replicante | Duplica no 1º impacto |
| 3 | Desestabilizador de RNA | Planta minas biológicas; Mouse 2 detona todas; cooldown 0,75 s | Lança-Minas de RNA | Arremessa minas (pavio 6 s) |
| 4 | BFG Imunológico | Projétil perfurante (atravessa todos); cooldown 5 s | BFG Imunológico Ômega | Perfura + explode no fim do trajeto |

### Sistema de Quiz Educativo

- **48 perguntas** sobre SUS, vigilância epidemiológica, vacinação, dengue, resistência antimicrobiana e microbiologia
- Aparece entre ondas com feedback educativo explicativo após cada resposta
- Embaralhamento por Fisher-Yates; histórico de 14 perguntas recentes evita repetição

### Progressão

| Recurso | Descrição |
| ------- | --------- |
| XP / Nível | Ganho por kills; sobe atributos do anticorpo |
| Pontos do SUS | Moeda de upgrades mid-run (HP máx., ataque, velocidade) |
| Score | Acumulado por toda a campanha; exibido na tela de vitória |
| Desbloqueáveis | 4 evoluções de arma (30 abates/slot), 3 skins de personagem, 3 skins de arma, 8 cosméticos |

### Power-ups (10 tipos)

`HP_RECOVERY` · `SPEED_BOOST` · `SHIELD` · `ATTACK_BOOST` · `POWERUP_MASK` (dano −40%) · `POWERUP_DISTANCING` (aura repulsora) · `POWERUP_RNA_GRENADE` (explosão em área) · `POWERUP_CYTOKINE` (cura + regeneração) · `POWERUP_SUPREME_ORB` (todos os buffs, 7 s) · `POWERUP_BARRIER` (escudo + máscara, 12 s)

### Save / Load

- 3 slots com metadados (nome, nível, score, onda, data)
- Quicksave F5 / Quickload F9 (assíncrono em thread)
- Formato `.txt` com header versionado (`AUDIO2`)

### Telas (20 estados — enum `GameScreen`)

Menu Principal · Seleção de Dificuldade · Tutorial · Gameplay · Pausa · Quiz · Upgrade · Arsenal · Skins · Cosméticos · Configurações · Transição de Mundo (cutscene) · Save/Load Select · Loading (dica educativa rotativa) · Game Over · Vitória · Admin/Debug

### Testes Automatizados

| Arquivo | Cobre |
| ------- | ----- |
| `test_rifle_range.c` | Alcance 1050 px, velocidade 620 px/s (6000 iterações) |
| `test_virus_waves.c` | Composição de ondas M2; escalamento HP/escudo; IDs 0–11 |
| `test_map_body.c` | Colisão do mapa do corpo humano |
| `test_core_spawns.c` | Posicionamento dos 4 Núcleos de Infecção |
| `test_cosmetics.c` | Sistema de cosméticos, 8 slots |
