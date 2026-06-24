# 10.1 Catálogo de Inimigos

> 12 tipos com IDs 0–11. IDs 0–4 são legados (save-compatíveis). IDs 5–11 são os tipos do sistema atual.

---

## Mundo 1 — Bactérias

| ID | Nome in-game | Tipo biológico | Comportamento | HP base | HP/onda | Velocidade | Dano extra | Tier |
| -- | ------------ | -------------- | ------------- | ------- | ------- | ---------- | ---------- | ---- |
| 5 | Coco | Cocos (forma esférica) | Melee direto | — | — | — | 0 | TIER_1 |
| 6 | Bacilo | Bacilo (forma de bastão) | Ranged | — | — | — | 0 | TIER_2 |
| 2 | Superbactéria KPC | KPC resistente a antibióticos | Ranged pesado / Boss | — | — | — | alto | TIER_3_BOSS |

### Comportamento Bacteria
- Perseguição direta com flanking (`flankSign = ±1`)
- Miniboss: sempre `ETYPE_KPC` em qualquer onda do Mundo 1
- Boss Mundo 1 (Onda 5): Superbactéria KPC com `HP = 1400 + onda×100`

---

## Mundo 2 — Vírus

| ID | Nome in-game | Tipo biológico | Comportamento | HP base | HP/onda | Escudo base | Escudo/onda | Escala | Tier |
| -- | ------------ | -------------- | ------------- | ------- | ------- | ----------- | ----------- | ------ | ---- |
| 10 | Rinovirus | Rinovírus (common cold) | Swarm — rápido, frágil | 12 | 3 | 8 | 2 | 0,60× | TIER_1 |
| 7  | Dengue | Vírus da dengue (envelopado) | Melee corpo-a-corpo | 30 | 9 | 28 | 8 | 1,05× | TIER_1 |
| 8  | Influenza | Influenzavirus (H1N1/H3N2) | Ranged projéteis | 22 | 6 | 28 | 8 | 1,00× | TIER_2 |
| 11 | Sarampo (mutante) | Paramyxovirus mutante | Elite — grande, resistente | 180 | 30 | 90 | 20 | 1,50× | TIER_3 |
| 9  | Coronavírus | SARS-CoV-2 | Boss — 3 fases de IA | 1400 | 100 | — | — | 1,15× | TIER_3_BOSS |

### Comportamento Vírus
- **Swarm (Rinovirus)**: movimentação em bando; recua quando HP < threshold (flee)
- **Melee (Dengue)**: pressão corpo-a-corpo constante; usa Capsídeo como "casca"
- **Ranged (Influenza)**: dispara `PROJ_VIRAL_SPORE` a 330 px/s com `chargeTimer`
- **Elite (Sarampo)**: lento mas resistente; escudo de 90–180 pts dependendo da onda; aplica Lentidão
- **Boss (Coronavírus)**:
  - `aiPhase 0`: invoca Núcleos de Infecção (boss invulnerável)
  - `aiPhase 1`: ataque de alcance médio
  - `aiPhase 2`: summon de minions a cada 6 s; 12 minions em onda de boss

---

## Composição de Ondas do Mundo 2 (Determinística)

| Onda | Inimigos comuns | Miniboss |
| ---- | --------------- | -------- |
| 1 | Apenas Rinovirus | Rinovirus líder (Swarm) |
| 2 | Rinovirus + Dengue | Dengue envelopado |
| 3 | Rinovirus + Dengue + Influenza | Sarampo mutante (Elite) |
| 4 | Todos os tipos + Sarampo | Sarampo mutante reforçado |
| 5 | Coronavírus (Boss) + 12 minions | — |

---

## Capsídeo Viral (Mechanic)

- Todo vírus tem `shieldHp = shieldBase + onda × shieldPerWave`
- Dano vai ao escudo primeiro; HP só é afetado após `shieldHp ≤ 0`
- `shieldHitFlash ≈ 0,18 s` — pisca ao receber dano no escudo
- **Counters**:
  - **Lâmina Bioelétrica** — ×3 de dano contra o escudo (quebra bem mais rápido)
  - **Power-up Desestabilizador de RNA** (drop) — a explosão **dissolve o capsídeo** direto antes de ferir a vida
  - ⚠️ A **arma** slot 3 (minas) NÃO ignora o escudo: o dano da mina passa pelo capsídeo normalmente

---

## Projéteis dos Inimigos

| Projétil | Velocidade | Efeito de status |
| -------- | ---------- | ---------------- |
| `PROJ_ACID_ARC` (Bacilo) | 220 px/s | Veneno 3 s |
| `PROJ_BULLET_SPREAD` (Legado Aedes) | 350 px/s | — |
| `PROJ_VIRAL_SPORE` (Influenza/Elite/Boss) | 330 px/s | — |
| `PROJ_VOID_BOLT` (Elite/Boss) | 400 px/s | Lentidão 2 s |
| `PROJ_BOSS_BULLET` (KPC Boss) | 300 px/s | — |

---

## Núcleos de Infecção (Boss Fase 3)

- 4 núcleos (`MAX_CORES`) distribuídos ao redor do boss
- Cada núcleo: 15–20 HP (escala por onda)
- Boss é **invulnerável** enquanto pelo menos 1 núcleo estiver ativo
- Só são danificados pela AoE corpo-a-corpo (raio 140 px)
- Distância mínima entre núcleos: 240 px; clearance do boss: 180 px

---

## Inimigos Legados (Save-Compatíveis)

| ID | Nome | Observação |
| -- | ---- | ---------- |
| 0 | SARS-CoV-2 | Melee balanceado (tipo legado) |
| 1 | Aedes | Ranged rápido (tipo legado) |
| 3 | T. cruzi | Melee (Chagas, tipo legado) |
| 4 | Tuberculose | Ranged (tipo legado) |
