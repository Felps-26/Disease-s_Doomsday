# 6.1 Princípios de Design

O design do jogo segue quatro princípios centrais, pensados especificamente para o contexto de **uso em sala de aula por estudantes do Ensino Médio do DF**:

---

## 1. Clareza

O jogador deve entender o que está acontecendo sem precisar ler um manual.

- HUD limpo com apenas as informações essenciais sempre visíveis (vida, onda, arma equipada, inimigos restantes)
- Nomes das doenças e dos inimigos exibidos diretamente no HUD (rótulo de órgão/doença)
- Hotbar de armas com indicador visual de cooldown e estado de evolução
- Banners temporários anunciam eventos importantes (início de onda, boss chegando, arma evoluída)

## 2. Feedback Imediato

Toda ação do jogador deve ter resposta visual e/ou sonora instantânea.

| Ação | Feedback |
| ---- | -------- |
| Acerto de ataque | Número de dano flutuante + partículas + screen shake |
| Tomar dano | Flash vermelho de tela + screen shake + knockback |
| Level-up | Banner + som de level-up |
| Evolução de arma desbloqueada | Banner anunciando a nova forma (uma única vez por campanha) |
| Resposta correta no quiz | Indicador visual verde + explicação educativa |
| Resposta errada no quiz | Indicador visual vermelho + explicação educativa (sem punição de gameplay) |
| Capsídeo atingido | Flash azul na barra do escudo (`shieldHitFlash` 0,15 s) |
| Boss invulnerável por núcleo | Banner de alerta |

## 3. Consistência

A linguagem visual é a mesma em todas as telas e contextos.

- **Vermelho** → inimigos bacterianos, perigo, dano
- **Violeta** → inimigos virais, capsídeo
- **Ciano** → jogador (anticorpo), destaques positivos
- **Verde** → vida, XP, resposta correta
- **Azul** → escudo, capsídeo do inimigo
- **Dourado** → pontos do SUS, XP, recompensas
- **Laranja** → boss, alertas críticos

Esta paleta é mantida consistentemente do gameplay ao HUD, dos menus às cutscenes.

## 4. Acessibilidade de Uso

O jogo precisa funcionar para estudantes sem experiência prévia com jogos de PC.

- **Tutorial obrigatório** de 3 etapas (movimento → combate → saída) antes da campanha
- **Controles simples**: WASD + mouse ou teclas numéricas — sem combinações complexas
- **3 níveis de dificuldade** configuráveis antes de cada partida — garante que alunos com diferentes habilidades possam progredir
- **Texto legível**: fonte bitmap em tamanhos de 14 a 48 px conforme contexto
- **Save manual e automático**: o aluno não perde progresso se a aula terminar
- **Sem penalização por resposta errada no quiz**: o objetivo é aprender, não punir

---

## Princípio Pedagógico Adicional

> **A mecânica deve reforçar o conceito.** Não é suficiente colocar o nome de uma doença num inimigo — o *comportamento* do inimigo deve evocar a realidade biológica da doença.

| Doença | Comportamento que reforça o conceito |
| ------ | ------------------------------------- |
| Dengue | Melee agressivo — o vírus precisa entrar nas células do hospedeiro |
| Influenza | Projéteis à distância — transmissão por gotículas e aerossóis |
| Rinovírus | Swarm de bando — transmissão em surtos rápidos e em grupo |
| Sarampo | Elite lento e resistente — alta virulência, baixa velocidade de progressão |
| KPC | Boss em 3 fases — resistência progressiva a múltiplas formas de combate |
| Capsídeo viral | Escudo que absorve dano antes do HP — proteção estrutural real do vírus |
