# 4.1 Requisitos Funcionais

> Baseados no código-fonte auditado em junho de 2026.

| ID   | Requisito                                                                            | Prioridade | Status        |
| ---- | ------------------------------------------------------------------------------------ | ---------- | ------------- |
| RF01 | O jogo deve apresentar dois mundos temáticos (Bactérias e Vírus)                     | Alta       | Implementado  |
| RF02 | Cada mundo deve conter 5 ondas progressivas; a onda 5 é sempre o chefe principal     | Alta       | Implementado  |
| RF03 | O sistema de Quiz deve aparecer entre as ondas com feedback educativo por pergunta    | Alta       | Implementado  |
| RF04 | O jogador deve poder salvar e carregar o progresso em 3 slots distintos              | Alta       | Implementado  |
| RF05 | As armas devem refletir conceitos biológicos reais com nomes e efeitos temáticos     | Média      | Implementado  |
| RF06 | O mapa deve representar o corpo humano com colisão por máscara de pixel              | Média      | Implementado  |
| RF07 | O jogo deve exibir mensagens educativas em banners, loading tips e cutscenes         | Alta       | Implementado  |
| RF08 | O sistema de progressão (XP, Nível, Pontos do SUS) deve funcionar entre ondas       | Alta       | Implementado  |
| RF09 | Os inimigos do Mundo 2 devem possuir escudo de capsídeo com pontos de vida próprios | Média      | Implementado  |
| RF10 | O jogo deve funcionar completamente sem conexão à internet                           | Alta       | Implementado  |
| RF11 | O jogo deve oferecer 3 níveis de dificuldade (Fácil, Médio, Difícil) com config. independente | Alta | Implementado |
| RF12 | O tutorial deve ser uma fase separada (mapa da seringa) com 3 etapas guiadas         | Alta       | Implementado  |
| RF13 | O quicksave (F5) e o quickload (F9) devem estar disponíveis durante o gameplay       | Média      | Implementado  |
| RF14 | O chefe do Mundo 2 deve ter 3 fases de comportamento (aiPhase 0/1/2) e núcleos de infecção | Alta | Implementado |
| RF15 | Cada slot de arma (1–4) deve evoluir automaticamente após 30 abates com aquele slot (4 evoluções, incl. Lâmina Bioelétrica) | Média | Implementado  |
| RF16 | O Rifle deve mudar de tipo entre mundos (Bacteriófago → Vacina) com bônus de +60%   | Média      | Implementado  |
| RF17 | Power-ups educativos devem incluir Máscara Hospitalar e Distanciamento Social        | Média      | Implementado  |
| RF18 | O jogo deve exibir metadados de save (nível, score, onda, data) na tela de carregamento | Média | Implementado |
| RF19 | O sistema de cosméticos deve oferecer 8 slots independentes por personagem           | Baixa      | Implementado  |
| RF20 | A transição entre Mundo 1 e Mundo 2 deve incluir cutscene educativa com diálogo      | Média      | Implementado  |
| RF21 | O quiz deve ter banco de 48 perguntas e evitar repetir as últimas 14 da sessão       | Alta       | Implementado  |
| RF22 | Os inimigos devem escalar HP, velocidade e escudo por onda e dificuldade             | Alta       | Implementado  |
| RF23 | O slot 3 (Desestabilizador de RNA) deve plantar minas detonáveis por tiro/melee ou Mouse 2 | Média | Implementado  |

---

## Detalhamento dos Principais Requisitos

### RF02 — Ondas
- Fórmula de inimigos comuns: `numEnemies = 8 + wave * 6` (máx. 60)
- Onda 5 (boss): 1 chefe + 12 minions em formação circular
- Ondas 1–4: inimigos comuns + 1 miniboss + escoltas

### RF04 — Save
- Formato: posição, HP, XP, nível, score, ataque, Pontos do SUS, onda, arma equipada, estado dos inimigos
- Header `AUDIO2` garante retrocompatibilidade entre versões

### RF09 — Capsídeo
- Dano vai ao escudo primeiro; HP do inimigo só é afetado após escudo ser zerado
- O **power-up** Desestabilizador de RNA (drop) dissolve o capsídeo direto antes de ferir a vida
- Lâmina Bioelétrica inflige 3× dano ao escudo

### RF11 — Dificuldade
- `DifficultyConfig` aplica multiplicadores independentes a: HP, dano, velocidade, alcance de detecção, chance de desvio, threshold de recuo, cadência do boss

### RF14 — Chefe do Vírus (Coronavírus)
- HP base: 1400 + wave × 100
- Invoca minions a cada 6 s
- Fase 3: 4 Núcleos de Infecção ao redor do chefe; boss invulnerável até destruí-los
