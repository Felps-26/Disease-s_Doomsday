# 4.2 Requisitos Não Funcionais

| ID    | Requisito                                                                    | Categoria        | Referência no Código |
| ----- | ---------------------------------------------------------------------------- | ---------------- | -------------------- |
| RNF01 | O jogo deve manter 60 FPS em hardware com OpenGL 3.3 e 512 MB de RAM        | Desempenho       | `SetTargetFPS(60)` em `main.c` |
| RNF02 | Pool de 250 partículas, 100 projéteis e 60 inimigos simultâneos sem queda de FPS | Desempenho  | `MAX_PARTICLES=250`, `MAX_PROJECTILES=100`, `MAX_ENEMIES=60` |
| RNF03 | O código deve ser modular com separação clara entre entities, systems, render e logic | Manutenibilidade | Estrutura de pastas `src/` |
| RNF04 | O build deve compilar e executar em Windows, Linux e macOS                   | Portabilidade    | `compilar.bat` + `Makefile` + `build.sh` |
| RNF05 | Todo conteúdo educativo deve ser scientificamente correto e revisado         | Qualidade        | 48 perguntas baseadas em OMS/MS/SES-DF |
| RNF06 | O jogo deve carregar em menos de 5 segundos em hardware padrão               | Desempenho       | Tela de loading com duração mínima configurável |
| RNF07 | O código-fonte deve ser versionado no GitHub com histórico rastreável        | Rastreabilidade  | Repositório Felps-26/Disease-s_Doomsday |
| RNF08 | A interface deve ser intuitiva: controles operacionais sem leitura de manual | Usabilidade      | Tutorial guiado de 3 etapas na seringa |
| RNF09 | O render deve usar supersampling 2× para imagem mais nítida em telas HD      | Qualidade Visual | `WORLD_SS = 2` em `main.c` |
| RNF10 | O save deve ser retrocompatível entre versões do jogo                        | Manutenibilidade | Header `AUDIO2`; campos de padding reservados |
| RNF11 | O binário compilado deve ter menos de 5 MB                                   | Portabilidade    | ~2,7 MB (Windows, MinGW) |
| RNF12 | A suite de testes deve cobrir ondas, projéteis, mapa e sistemas críticos     | Testabilidade    | 5 arquivos em `tests/`, sem dependência externa |
| RNF13 | O jogo não deve coletar nenhum dado pessoal do usuário                       | Privacidade / LGPD | Saves locais em `.txt`; nenhum network call |
| RNF14 | A dificuldade deve ser configurável em qualquer nova partida sem reiniciar   | Usabilidade      | `SCREEN_DIFFICULTY_SELECT` antes de cada campanha |

---

## Parâmetros de Desempenho Verificados

| Parâmetro | Valor configurado |
| --------- | ----------------- |
| FPS alvo | 60 |
| Resolução interna (render) | 2560 × 1440 (supersampling 2×) |
| Resolução de saída | 1280 × 720 |
| Tamanho máximo do mapa | 5657 × 5657 px |
| Máximo de entidades simultâneas | 60 inimigos + 100 projéteis + 250 partículas + 32 textos de dano + 20 power-ups |
| Grade de colisão | 452 × 452 células (~12,5 px/célula) |
| Shader | GLSL customizado (distorção + vinheta); aplicado apenas no gameplay |
