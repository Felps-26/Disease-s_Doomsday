# 10.2 Catálogo de Armas

> **4 armas-base** (slots 1–4), disponíveis por progressão de nível. Cada slot **evolui**
> automaticamente após **30 abates com aquele slot** (`WEAPON_EVOLVE_KILLS = 30`), liberando
> uma forma mais forte — total de **8 formas** (`WEAPON_COUNT = 8`). Pressionar o número do slot
> alterna entre a base e a evolução. Cada arma tem temática biológica real.

---

## Slot 1 — Espada-Seringa → Lâmina Bioelétrica

### Base: Espada-Seringa

| Atributo | Valor |
| -------- | ----- |
| **Conceito biológico** | Seringa hipodérmica / anticorpo neutralizante |
| **Tipo** | Corpo-a-corpo AoE (combo: estocada frontal + corte lateral) |
| **Cooldown** | 0,24 s |
| **Dano base** | 15 + `attackPower` do jogador |
| **Área de efeito** | 360° ao redor do jogador, raio **140 px** |
| **Knockback** | 55 px (comum), 22 px (miniboss), 8 px (boss) |
| **Disponibilidade** | Desde o início |

### Evolução: Lâmina Bioelétrica *(30 abates no slot 1)*

| Atributo | Valor |
| -------- | ----- |
| **Conceito biológico** | Potencial bioelétrico das membranas / ação anti-capsídeo |
| **ID interno** | `WEAPON_BIOBLADE` (5) |
| **Mecânica especial** | **×3 de dano contra o escudo de capsídeo** (`danoTotal *= 3` quando `shieldActive`) |
| **Dano a Núcleos** | Sim, via AoE corpo-a-corpo |
| **Desbloqueio** | 30 abates no slot 1; anúncio via banner uma única vez por campanha (`bioBladeAnnounced`) |

---

## Slot 2 — Rifle Contextual → Rifle Vetorial Replicante

### Base: Rifle (muda por Mundo)

| Atributo | Mundo 1 — Rifle de Bacteriófagos | Mundo 2 — Rifle de Vacina |
| -------- | -------------------------------- | ------------------------- |
| **Conceito** | Bacteriófago — vírus que infecta bactérias | Vacina / anticorpo específico antiviral |
| **Projétil** | `PROJ_PLAYER_PHAGE` | `PROJ_PLAYER_VACCINE` |
| **Bônus** | +60% dano vs. **bactérias** | +60% dano vs. **vírus** |
| **Dano base** | 8 + `attackPower` | 8 + `attackPower` |
| **Cooldown** | 0,28 s | 0,28 s |
| **Alcance máximo** | 1050 px | 1050 px |

> A troca de tema é feita por `SetWeaponWorld()` ao entrar em cada Mundo.

### Evolução: Rifle Vetorial Replicante *(30 abates no slot 2)*

| Atributo | Valor |
| -------- | ----- |
| **ID interno** | `WEAPON_RIFLE_EVOLVED` (6) |
| **Projétil** | `PROJ_PLAYER_RIFLE_EVOLVED` |
| **Dano base** | 10 + `attackPower` |
| **Cooldown** | 0,24 s |
| **Mecânica especial** | O **primeiro impacto gera um projétil extra** de mesmo dano (duplicação única) |

---

## Slot 3 — Desestabilizador de RNA → Lança-Minas de RNA

> ⚠️ **Mudança de design:** o slot 3 **não é mais uma granada arremessada** — agora **planta minas
> biológicas** no chão. As minas só detonam quando atingidas por tiros/melee ou pelo **Mouse 2**
> (detona todas de uma vez), criando uma decisão tática.

### Base: Desestabilizador de RNA

| Atributo | Valor |
| -------- | ----- |
| **Conceito biológico** | Interferência de RNA / inibição da replicação |
| **Tipo** | Plantador de minas (`BioMine`, até `MAX_BIOMINES = 12` ativas) |
| **Cooldown** | 0,75 s |
| **Dano base por mina** | 48 + `attackPower` |
| **Posição da mina** | ~52 px à frente do jogador |
| **Gatilho** | Tiro/melee do jogador na mina **ou** Mouse 2 (detona todas) |
| **Disponibilidade** | Desde o início |

### Evolução: Lança-Minas de RNA *(30 abates no slot 3)*

| Atributo | Valor |
| -------- | ----- |
| **ID interno** | `WEAPON_RNA_LAUNCHER` (7) |
| **Cooldown** | 0,68 s |
| **Dano base por mina** | 58 + `attackPower` |
| **Posição da mina** | **Arremessada a ~310 px** de distância |
| **Pavio** | Explode sozinha em **6 s** (`RNA_LAUNCHER_FUSE`) ou com Mouse 2 |

---

## Slot 4 — BFG Imunológico → BFG Imunológico Ômega

### Base: BFG Imunológico

| Atributo | Valor |
| -------- | ----- |
| **Conceito biológico** | Resposta imune sistêmica — mobilização total do organismo |
| **Tipo** | Canhão perfurante |
| **Projétil** | `PROJ_PLAYER_BFG` |
| **Cooldown** | 5,0 s |
| **Dano base** | 100 + `attackPower` |
| **Mecânica especial** | **Perfurante** — atravessa e atinge todos os inimigos na trajetória |
| **Screen shake** | 0,8 s |
| **Disponibilidade** | Desde o início |

### Evolução: BFG Imunológico Ômega *(30 abates no slot 4)*

| Atributo | Valor |
| -------- | ----- |
| **ID interno** | `WEAPON_BFG_EVOLVED` (8) |
| **Projétil** | `PROJ_PLAYER_BFG_EVOLVED` |
| **Cooldown** | 5,8 s |
| **Dano base** | 115 + `attackPower` |
| **Mecânica especial** | Perfura **e detona em explosão de área no fim do trajeto** |

---

## Skins de Arma

| Skin | Nome in-game | Primary | Secondary |
| ---- | ------------ | ------- | --------- |
| 0 | PADRAO | Azul-imune | Branco-gelo |
| 1 | PLASMA | Magenta | Ciano |
| 2 | TOXICA | Verde-ácido | Lima |

A cor secundária é usada nos **textos de dano flutuante** e nos traços visuais do projétil.
Skins de arma são puramente cosméticas (não afetam o balanceamento).

---

## Tabela Comparativa

| Slot | Base | CD base | Evolução (30 abates) | Efeito da evolução |
| ---- | ---- | ------- | -------------------- | ------------------ |
| 1 | Espada-Seringa | 0,24 s | Lâmina Bioelétrica | Quebra capsídeo bem mais rápido |
| 2 | Rifle (Phage/Vacina) | 0,28 s | Rifle Vetorial Replicante | Duplica no 1º impacto |
| 3 | Desestabilizador de RNA | 0,75 s | Lança-Minas de RNA | Arremessa minas (pavio 6 s) |
| 4 | BFG Imunológico | 5,0 s | BFG Imunológico Ômega | Perfura + explosão final |
