# 10.3 Catálogo de Power-ups

> **10 tipos** de power-up (`PowerUpType`, com `POWERUP_TYPE_COUNT` no fim). Drops aparecem no mapa
> em quantidade `4 + onda` (máx. 10) por geração de onda.

---

## Power-ups Base (4 tipos)

| Tipo | `PowerUpType` | Conceito biológico | Efeito no jogo |
| ---- | ------------- | ------------------ | -------------- |
| Recuperação de HP | `HP_RECOVERY` | Nutrientes / glóbulos vermelhos | Restaura pontos de vida imediatamente |
| Boost de Velocidade | `SPEED_BOOST` | Adrenaline / resposta autonômica | Aumenta velocidade do anticorpo temporariamente |
| Escudo | `SHIELD` | Complemento imune / barreira de anticorpos | Absorve próximos danos antes do HP; partículas SKYBLUE ao absorver |
| Boost de Ataque | `ATTACK_BOOST` | Citocinas pró-inflamatórias / resposta imune adaptativa | Dobra todo o dano causado enquanto ativo |

---

## Power-ups Educativos / Expansão (6 tipos)

| Tipo | `PowerUpType` | Conceito de saúde pública real | Efeito mecânico |
| ---- | ------------- | ------------------------------ | --------------- |
| Máscara Hospitalar | `POWERUP_MASK` | EPI obrigatório em ambientes hospitalares | Reduz todo dano recebido em **40%** (`maskTimer` → multiplicador 0,6) |
| Distanciamento Social | `POWERUP_DISTANCING` | Medida de controle epidemiológico não-farmacológico | Aura repulsora que afasta inimigos por ~10 s (`distancingTimer`) |
| Desestabilizador de RNA | `POWERUP_RNA_GRENADE` | Interferência de RNA (terapia gênica) | **Explosão imediata em área** ao redor do herói (`DetonateRNAGrenade`) |
| Citocina de Estabilização | `POWERUP_CYTOKINE` | Citocinas anti-inflamatórias / imunomodulação | Cura imediata (+20 HP) + regeneração contínua por ~6 s (`regenTimer`) |
| Orbe Supremo | `POWERUP_SUPREME_ORB` | Resposta imune total / pico imunológico | Por ~7 s ativa **todos** os sistemas: velocidade, ataque ×, escudo, regen e bônus de dano (`supremeTimer`) |
| Barreira de Plasma | `POWERUP_BARRIER` | Imunidade reforçada (escudo de anticorpos) | Por ~12 s combina escudo + máscara (dano −40% e absorção) |

> **Drops por mundo:** os itens Máscara/Distanciamento aparecem com tema do Mundo 1 (Bactérias).
> A distribuição de raridade é definida em `RandomDropType()` (HP é o mais comum; Orbe Supremo, o mais raro).

---

## Mecânica de Geração

- Quantidade por onda: `4 + onda_atual` (máximo 10)
- Posição: spawn dentro do mapa do corpo humano com margem de 30 px das bordas
- Spawn após início de cada onda junto dos inimigos

---

## Timers Ativos (campo da struct Player)

| Campo | Comportamento |
| ----- | ------------- |
| `speedTimer` | > 0 → velocidade aumentada |
| `shieldTimer` | > 0 → escudo ativo; absorve dano em vez do HP |
| `attackBoostTimer` | > 0 → dano × 2 em todas as armas |
| `maskTimer` | > 0 → dano recebido × 0,6 |
| `distancingTimer` | > 0 → aura repulsora ativa |
| `regenTimer` | > 0 → regen por `regenAccum` (acumulador fracional para HP inteiro) |
| `supremeTimer` | > 0 → Orbe Supremo: +25% velocidade, +35% dano e demais buffs simultâneos |
| `poisonTimer` | > 0 → dano de veneno por tick |
| `slowTimer` | > 0 → velocidade reduzida (debuff de inimigos) |

---

## Relação com Conceitos do SUS

| Power-up | Campanha/Programa do SUS relacionado |
| -------- | ------------------------------------ |
| Máscara Hospitalar | PCIH (Prevenção e Controle de Infecções Hospitalares) / ANVISA |
| Distanciamento Social | Plano Nacional de Contingência para Emergências em Saúde Pública |
| Citocina | Pesquisa em imunobiológicos — Instituto Butantan / Fiocruz |
| Recuperação de HP | Atenção Básica / PSF — cuidado preventivo e recuperação |
| Escudo | Vacinação — programa NIP/PNI |
| Boost de Ataque | Vigilância Epidemiológica ativa — SINAN / CVE |
