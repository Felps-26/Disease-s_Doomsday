# 2.6 How — Como?

## Tecnicamente

### Pipeline de Render

1. O jogo renderiza em um `RenderTexture2D` de **2560 × 1440 px** (supersampling 2×)
2. O resultado é downscalado bilinearmente para a janela de **1280 × 720 px**
3. Durante o gameplay e pausa, um **shader GLSL customizado** aplica distorção orgânica + vinheta biológica
4. O shader não é aplicado no tutorial (ambiente mais limpo para ensinar controles)

### Arquitetura de Módulos

| Módulo | Responsabilidade |
| ------ | ---------------- |
| `src/logic/fsm.c` | Máquina de estados com 20 telas (enum `GameScreen`); gerencia todas as transições |
| `src/logic/update_gameplay.c` | Loop de atualização: input → física → colisão → IA → partículas |
| `src/render/render_gameplay.c` | Desenho do mundo, entidades, HUD, efeitos visuais |
| `src/systems/wave_manager.c` | Geração determinística de ondas; escalamento por wave e dificuldade |
| `src/systems/combat_system.c` | Fórmulas de dano, I-frames, capsídeo, knockback, drops de XP |
| `src/entities/` | Player, Enemy, Projectiles — structs + update |
| `src/telas/` | Lógica de cada tela (quiz, arsenal, skins) |
| `Assets/@models/` | Funções de desenho procedural para player, inimigos e armas |

### Sistema de IA dos Inimigos

- **5 comportamentos**: MELEE, RANGED, SWARM, ELITE, BOSS
- Inimigos comuns: perseguição direta + flanking (sinal `-1` ou `+1`)
- Inimigos frágeis (Swarm): recuam quando com baixo HP (`fleeTimer`)
- Boss: 3 fases de IA (`aiPhase 0/1/2`); invocação de minions a cada 6 s
- Percepção: `detectionRange` + memória de posição (`aggroMemoryTime` segundos)
- Dodging por projétil: `dodgeChance` configurável por dificuldade

### Sistema de Física & Colisão

- Colisão de mapa via máscara de pixels gerada por `bake_collision_mask.py`
  - Grade de **452 × 452** células (~12,5 px/célula)
  - Threshold de luminância: 12 (pixels escuros = parede)
  - Raios de colisão: Player = 20 px, Enemy = 35 px, Boss = 100 px
- Colisões entidade-entidade: círculos; knockback em pixels fixos por tier

### Áudio

- Sistema dual-track: `musicA` / `musicB` com crossfade suave
- Fade começa 12 segundos antes do fim de cada faixa
- Volume separado: `musicVolume` e `sfxVolume` (0..1), persistidos no save

### Pipeline de Save

```
config.txt   → "AUDIO2 musicVol sfxVol skinId weaponSkinId difficulty ..."
save_slot_N  → posX posY hp maxHp xp level score attack susPoints wave weapon numEnemies [per-enemy data...]
```

- Retrocompatibilidade garantida via header de versão
- Quicksave/Quickload em thread assíncrona (teclas F5/F9)

### Testes

| Arquivo | Cobre |
| ------- | ----- |
| `test_rifle_range.c` | Projétil Phage: alcance máximo 1050 px, velocidade 620 px/s (6000 iterações aleatórias) |
| `test_virus_waves.c` | Composição de ondas Mundo 2; escalamento de HP/escudo por wave; compatibilidade de IDs 0–11 |
| `test_map_body.c` | Colisão do mapa do corpo humano |
| `test_core_spawns.c` | Posicionamento dos Núcleos de Infecção (boss fase 3) |
| `test_cosmetics.c` | Sistema de cosméticos, 8 slots |

---

## Pedagogicamente

1. **Mapeamento mecânica ↔ conceito**:
   - Capsídeo viral → escudo de pontos que precisa ser destruído primeiro
   - Rifle de Bacteriófago → vírus que infecta bactérias (bônus só no Mundo 1)
   - Rifle de Vacina → anticorpos específicos (bônus só no Mundo 2)
   - Máscara Hospitalar → power-up que reduz dano recebido em 40%
   - Distanciamento Social → aura repulsora que afasta inimigos
   - Citocina → regeneração de HP (resposta imune natural)
   - BFG Imunológico → resposta imune sistêmica (atinge todos os patógenos)

2. **Quiz com 48 perguntas** cobrindo SUS, ESF, vacinação, arboviroses, resistência antimicrobiana e microbiologia; feedback explicativo após cada resposta

3. **Mensagens educativas** em banners, loading tips e cutscene de transição de mundo

4. **Tom informativo e acionável** — conteúdo baseado em OMS, Ministério da Saúde e SES-DF
