# 8.2 Backlog de Produto

| Prioridade | Item | Status |
| ---------- | ---- | ------ |
| Alta | MVP jogável (Mundo 1 completo com 5 ondas + boss KPC) | Concluído |
| Alta | Sistema de Quiz educativo (48 perguntas) | Concluído |
| Alta | Save/Load multi-slot (3 slots + quicksave F5/F9 assíncrono) | Concluído |
| Alta | Tutorial na seringa (3 etapas guiadas) | Concluído |
| Alta | Sistema de dificuldade (Fácil / Médio / Difícil) | Concluído |
| Alta | Mundo 2 — Vírus (5 tipos, capsídeo, boss Coronavírus 3 fases) | Concluído |
| Média | Boss fase 3 — Núcleos de Infecção (4 núcleos, boss invulnerável) | Concluído |
| Média | Evolução de armas (4 slots → 4 evoluções aos 30 abates/slot) | Concluído |
| Média | Mapa do corpo humano (5657×5657, colisão por máscara de pixel) | Concluído |
| Média | Pipeline de sprites (render procedural + suporte PNG) | Concluído |
| Média | Cosmetics system (8 slots; 3 skins de personagem; 3 skins de arma) | Concluído |
| Média | Cutscene de transição entre Mundo 1 e Mundo 2 | Concluído |
| Média | HUD completo (radar, barra de chefe, hotbar, rótulo de doença) | Concluído |
| Média | Testes automatizados (5 suites em C puro) | Concluído |
| Baixa | Sprites PNG finais (substituir render procedural) | Em produção |
| Baixa | Versão Web via WebAssembly (Emscripten) | Planejado |
| Baixa | Ranking online com Pontos do SUS | Planejado |
| Baixa | Modo multiplayer local | Não iniciado |

---

## Testes Automatizados

| Teste | Cobre | Status |
| ----- | ----- | ------ |
| `test_rifle_range.c` | Projétil Phage: alcance 1050 px, velocidade 620 px/s | Implementado |
| `test_virus_waves.c` | Composição de ondas Mundo 2; escalamento de HP/escudo | Implementado |
| `test_map_body.c` | Colisão do mapa do corpo | Implementado |
| `test_core_spawns.c` | Posicionamento dos 4 Núcleos de Infecção | Implementado |
| `test_cosmetics.c` | Sistema de cosméticos (8 slots) | Implementado |
