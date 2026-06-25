# 8.5 Limitações Conhecidas

> Transparência sobre o que o projeto ainda não entrega ou entrega de forma incompleta.

---

## Limitações do Produto Atual (v1.0)

| Limitação | Impacto | Status |
| --------- | ------- | ------ |
| **Sprites PNG finais não implementados** | Todos os personagens e inimigos são renderizados proceduralmente em C puro — sem arte pixel-art finalizada | Em produção |
| **Sem versão Web** | Requer instalação local (Raylib + compilação); não roda direto no navegador | Planejado (Emscripten) |
| **Sem modo multiplayer** | Apenas um jogador por vez; sem cooperativo ou competitivo local/online | Não iniciado |
| **Sem ranking online** | Score é local — não há comparação entre jogadores ou turmas | Planejado |
| **Sem acessibilidade para daltonismo** | A paleta usa vermelho vs. azul vs. verde como código de informação; sem modo alternativo | Não iniciado |
| **Sem legendas ou narração em áudio** | Conteúdo educativo é exclusivamente visual/textual | Não iniciado |
| **Sem suporte a controle de videogame** | Apenas teclado + mouse | Não planejado |
| **Save não criptografado** | Arquivos `.txt` podem ser editados manualmente pelo usuário | Intencional (uso educacional local) |

---

## Limitações de Escopo Educacional

| Limitação | Observação |
| --------- | ---------- |
| **Sem avaliação formal de aprendizagem** | O quiz registra acertos/erros in-game, mas não exporta relatório para o professor | Futuro |
| **Sem painel do professor** | Não há interface para o educador configurar quais doenças abordar ou ver desempenho da turma | Futuro |
| **Conteúdo fixo** | Banco de 48 perguntas é estático — sem editor de perguntas ou personalização por professor | Futuro |
| **Simplificação científica intencional** | Conceitos biológicos são metáforas lúdicas, não simulações precisas — adequado para Ensino Médio, não para graduação em saúde | Intencional |

---

## O que não é uma limitação (mas parece ser)

| Item | Esclarecimento |
| ---- | -------------- |
| "Não tem gráficos profissionais" | O render procedural é intencional e funcional — sprites PNG finais estão em produção |
| "Não tem trilha sonora elaborada" | Usa dual-track com crossfade; qualidade adequada para o contexto |
| "Roda só offline" | É uma **vantagem** para escolas sem internet estável, não uma limitação |
