# 6.5 HUD (Heads-Up Display)

## Layout Visual

```
┌──────────────────────────────────────────────────────────┐
│  [Vida: ████████░░] [Escudo]  [XP: ██░░░░] Nível: 3     │
│  Órgão: Pulmões  |  Doença: Pneumonia Bacteriana         │
│  Onda: 3/5  Inimigos: 7                                  │
│                                                          │
│                  [Área de jogo]                          │
│                  shader: distorção orgânica + vinheta    │
│                                                          │
│  [Radar]                  [Score: 1250]  [SUS: 31 pts]  │
│  [1:Seringa] [2:Rifle] [3:Minas RNA] [4:BFG]            │
│  (slot evoluído após 30 abates) · cooldown por arma     │
└──────────────────────────────────────────────────────────┘
```

## Elementos do HUD

| Elemento | Descrição | Valores |
| -------- | --------- | ------- |
| **Barra de Vida** | HP atual / HP máximo com cor `#44FF44` | Escala dinâmica com nível |
| **Escudo** | Visível apenas quando power-up Shield está ativo | Cor `#4488FF` |
| **Barra de XP** | Progresso até o próximo nível; cor dourada `#FFD700` | Resets no level-up |
| **Nível** | Número inteiro do nível atual | Acumulado |
| **Onda** | Onda atual / total por mundo (ex: 3/5) | 1–5 por mundo |
| **Inimigos** | Contagem de inimigos restantes na onda | 0–60 |
| **Score** | Pontuação acumulada da campanha | Cresce por kill |
| **Pontos do SUS** | Moeda de upgrade; gasta em `SCREEN_UPGRADE` | Acumulado |
| **Hotbar de Armas** | 4 slots; indica arma equipada e cooldown visual | Slot evolui após 30 abates (pressionar o nº alterna base ↔ evolução) |
| **Radar** | Mini-mapa mostrando posição do jogador no corpo | |
| **Banners** | Mensagem central temporária (início de onda, boss, unlock) | Desaparece automaticamente |
| **Textos de dano** | Números flutuantes (+55 px/s por 0,7 s) acima de inimigos atingidos | Cor = skin de arma |
| **Flash de dano** | Tela vermelha por `hurtFlashTimer` ao tomar dano | 0,3 s |
| **Screen Shake** | Câmera oscila por `screenShake` após ataques fortes | 0,25–0,8 s conforme arma |

## Indicadores de Status (Buffs / Debuffs)

| Status | Ícone | Duração típica | Efeito visual |
| ------ | ----- | -------------- | ------------- |
| Speed Boost | ↑ velocidade | variável | rastro mais longo |
| Attack Boost | ⚔ × 2 | variável | dano dobrado |
| Máscara Hospitalar | 🩺 | variável | dano recebido −40% |
| Distanciamento Social | ↔ | variável | aura repulsora ao redor |
| Veneno | ☠ | 3 s (ácido) | HP drena lentamente |
| Lentidão | ↓ velocidade | 2 s (bolt) | movimento reduzido |
| Regeneração (Citocina) | ♥ | ~6 s | HP sobe lentamente |
| Orbe Supremo | 🌈 | 7 s | todos os buffs ativos (velocidade/dano/escudo/regen) |
| Barreira de Plasma | 🔵 | 12 s | escudo + máscara reforçados |
| Capsídeo (inimigo) | 🔵 | até ser destruído | barra azul separada do HP |

## Feedback Visual de Combate

| Evento | Efeito |
| ------ | ------ |
| Acerto corpo-a-corpo | Splash de partículas na cor da skin de arma + screen shake 0,25 s |
| Acerto de projétil | Screen shake 0,3 s |
| Golpe de inimigo corpo-a-corpo | Flash vermelho + screen shake 0,4 s + knockback |
| Golpe de projétil de inimigo | Flash vermelho + screen shake menor |
| Escudo (power-up) absorve | Partículas SKYBLUE sem flash vermelho |
| BFG disparado | Screen shake intenso 0,8 s |
| Capsídeo atingido | Pisca `shieldHitFlash` por 0,15 s |
| Boss invocando minions | Banner de alerta |
