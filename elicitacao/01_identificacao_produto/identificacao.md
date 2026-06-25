# 1. Identificação do Produto

| Campo | Informação |
| ----- | ---------- |
| **Nome do Produto** | Disease's Doomsday |
| **Tipo** | Jogo educativo digital — ação 2D _top-down_ |
| **Linguagem** | C (padrão C11) |
| **Biblioteca gráfica** | Raylib 5.x (licença zlib, open-source) |
| **Plataforma-alvo** | Windows / Linux / macOS |
| **Setor** | Educação em Saúde Pública — palco central: **Distrito Federal** |
| **Tema** | Vigilância Epidemiológica e prevenção de doenças infecciosas nas **escolas do DF** |
| **Repositório** | Felps-26/Disease-s_Doomsday (GitHub) |
| **Status** | v1.0 Concluído |

---

## Stack Técnica

| Camada | Tecnologia / Decisão |
| ------ | -------------------- |
| **Render** | Raylib — OpenGL 3.3 / ES 2.0 via abstração |
| **Supersampling** | Render em 2560 × 1440 (2×), downscale bilinear para 1280 × 720 |
| **Áudio** | Raylib + miniaudio (embutido); dual-track crossfade com fade de 12 s |
| **Shader** | Shader GLSL customizado: distorção orgânica + vinheta biológica |
| **Build (Windows)** | MinGW / GCC via `compilar.bat` |
| **Build (Unix)** | GCC via `Makefile` + `build.sh` |
| **Save/Load** | Arquivos `.txt` simples; header versionado (`AUDIO2`) |
| **Testes** | 5 suites em C puro (sem framework externo): `test_rifle_range`, `test_virus_waves`, `test_map_body`, `test_core_spawns`, `test_cosmetics` |
| **Ferramenta extra** | `bake_collision_mask.py` — gera máscara de colisão a partir do PNG do mapa |

---

## Arquitetura de Pastas

```
Disease's Doomsday/
├── Game/
│   ├── src/
│   │   ├── entities/        player, enemy, projectiles
│   │   ├── systems/         combat_system, wave_manager
│   │   ├── logic/           fsm (20 telas), update_gameplay
│   │   ├── render/          render_gameplay
│   │   ├── telas/           tela_quiz, tela_upgrade, telas
│   │   ├── gameplay/        input_controller, spatial_grid
│   │   └── assets/          asset_manager, sprite_manager
│   ├── include/             headers públicos (game.h, gameplay.h, telas.h)
│   ├── Assets/
│   │   ├── @models/         player_model, enemy_model, weapons_model
│   │   ├── Maps/            map_body (campanha), map_seringa (tutorial)
│   │   ├── Sprites/         spritesheet por entidade
│   │   └── SFX/             efeitos sonoros
│   ├── Saves/               config.txt + save_slot_*.txt
│   └── tests/               5 suites de teste em C puro
├── README.md
└── elicitacao/              documentação completa do projeto
```

---

## Proposta Educacional — Escolas do DF

O jogo é concebido como **ferramenta pedagógica** para uso em laboratórios de informática e salas de aula de Ciências e Biologia nas escolas do Distrito Federal. O conteúdo educativo é baseado em fontes do SUS, OMS, SES-DF e Ministério da Saúde, e cobre temas diretamente conectados à realidade epidemiológica local:

- Dengue e arboviroses — endêmicas no DF, transmitidas pelo *Aedes aegypti*
- Resistência antimicrobiana (KPC) — presente nos hospitais de referência regional do DF
- Sistema Imune, bacteriófagos, vacinas — conteúdo de Biologia do Ensino Médio
- SUS, ESF, vigilância epidemiológica — cidadania e saúde pública
