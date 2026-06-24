# 6.6 Sistema de Feedback Visual e Sonoro

## Partículas

- Pool de **250 partículas** gerenciado em `update_gameplay.c`
- Função `SpawnParticleExplosion(pos, color, count, minSpeed, maxSpeed, size, life)`
- Partículas emitidas radialmente com: ângulo aleatório, velocidade em `[minSpeed, maxSpeed]`, vida em `[life ± 0,2 s]`
- Usadas em: slash, acerto de projétil, morte de inimigo, Escudo absorvendo dano

## Textos de Dano Flutuantes

| Parâmetro | Valor |
| --------- | ----- |
| Velocidade de subida | 55 px/s |
| Duração | 0,7 s |
| Offset inicial | +30 px acima do inimigo |
| Jitter horizontal | ± 12 px aleatório |
| Cor | Cor secundária da skin de arma (melee) ou vermelho (dano recebido) |
| Pool máxima | 32 textos simultâneos (`MAX_DAMAGE_TEXTS`) |

## Screen Shake

| Evento | Intensidade |
| ------ | ----------- |
| Slash (Espada-Seringa / Bioblade) | 0,25 s |
| Acerto de projétil | 0,3 s |
| Colisão com inimigo | 0,4 s |
| BFG disparado | 0,8 s |

## Banners / Toasts

- Função `ShowBanner(msg, sub, color, duration)`
- `bannerMsg`: texto principal (até 48 chars)
- `bannerSub`: subtítulo (até 64 chars)
- Aparecem em: início de onda, entrada de boss, desbloqueio de arma, level-up, invocação de minions

## Animações de Personagem

| Animação | Detalhe |
| -------- | ------- |
| Rastro de movimento | `trail[10]` posições históricas; efeito ghosting |
| Squash & Stretch | `squashX`, `squashY` no jogador; elasticidade ao atacar/receber dano |
| Spawn de inimigo | `spawnAnim` — expansão de escala 0→1 ao aparecer |
| Ataque de inimigo | `attackAnim` — antecipação de rotação ou lunging |
| Morph de tela | `UIEase(t)` cúbico-out; `screenAnim` reinicia a 0 em cada transição |

## Slash Visual (Melee)

- `slashAnimTimer`: ~0,24 s (coincide com o cooldown do melee)
- `slashAnimPos`: posição do jogador no momento do ataque
- `slashAnimRadius`: 140 px (mesma área do hitbox)
- Partículas radiais em arco 360° na cor da skin de arma

## Feedback Sonoro

- Volume de música e SFX independentes (`musicVolume`, `sfxVolume`), range 0..1
- Crossfade dual-track: transição suave 12 s antes do fim de cada faixa
- SFX: por evento de combate (acerto, morte, level-up, boss spawn)
- Configurável na tela `SCREEN_SETTINGS` e persistido em `config.txt`
