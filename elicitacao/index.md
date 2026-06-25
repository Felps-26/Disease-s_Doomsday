# Documento de Elicitação — Disease's Doomsday

> **Disciplina:** Projeto Integrador
> **Data:** Junho de 2026
> **Versão:** 3.0 — Jogo Concluído

---

## Síntese do Produto

**Disease's Doomsday** é um jogo de ação 2D _top-down_ em C + Raylib onde o jogador controla um Anticorpo dentro do corpo humano, eliminando patógenos onda após onda enquanto aprende sobre vigilância epidemiológica e saúde pública.

O palco central é o **Distrito Federal** — onde os surtos anuais de dengue e a resistência antimicrobiana em hospitais de referência tornam o tema urgente e reconhecível. O jogo é pensado para uso em **aulas de Ciências e Biologia nas escolas do DF**, funcionando como ferramenta pedagógica interativa alinhada à BNCC.

| Métrica | Valor |
| ------- | ----- |
| Resolução | 1280 × 720 px (supersampling 2× em 2560 × 1440) |
| FPS alvo | 60 |
| Tamanho do mapa | 5657 × 5657 px (corpo humano) |
| Mundos | 2 (Bactérias → Vírus) |
| Ondas totais | 10 (5 por mundo) + 2 chefes |
| Tipos de inimigo ativos | 7 (Coco, Bacilo, KPC · Rinovírus, Dengue, Influenza, Sarampo, Coronavírus) |
| Armas | 8 formas (4 slots-base + 4 evoluções aos 30 abates/slot) |
| Power-ups | 10 tipos |
| Perguntas no quiz | 48 |
| Telas do jogo | 20 estados (enum `GameScreen`) |
| Slots de save | 3 manuais + quicksave F5/F9 |
| Dificuldades | 3 (Fácil / Médio / Difícil) |
| Testes automatizados | 5 suites em C puro |
| Status | **v1.0 Concluído** |

---

## Índice

| Seção | Conteúdo |
| ----- | -------- |
| [01 — Identificação do Produto](01_identificacao_produto/identificacao.md) | Nome, tipo, stack, plataforma, tema |
| [02 — Análise 5W2H](02_analise_5w2h/) | What · Why · Who · When · Where · How · How Much |
| [03 — Stakeholders](03_stakeholders/) | Mapa e detalhamento |
| [04 — Requisitos](04_requisitos/) | Funcionais · Não Funcionais · Regras de Negócio |
| [05 — Custo Computacional](05_custo_computacional/) | Hardware · Build · Operacional · Cloud |
| [06 — Plano de UX/UI](06_ux_ui/) | Design System · Paleta · Tipografia · Telas · HUD · Feedback |
| [07 — Segurança da Informação](07_seguranca/) | Risco local · Risco servidor · Política de dados |
| [08 — Product Management](08_product_management/) | Metodologia · Backlog · Definition of Done |
| [09 — Referências](09_referencias/referencias.md) | Fontes e documentos base |
| [10 — Catálogo de Mecânicas](10_catalogo_mecanicas/) | Inimigos · Armas · Power-ups |
