# 8.4 Métricas de Sucesso

> Como saberemos se o jogo atingiu seu objetivo educacional?

O sucesso do projeto é medido em dois eixos: **técnico** (o produto funciona como especificado?) e **educacional** (o conteúdo chega ao aluno de forma efetiva?).

---

## Métricas Técnicas (verificáveis agora)

| Métrica | Meta | Status |
| ------- | ---- | ------ |
| Jogo roda em 60 FPS em hardware mínimo (OpenGL 3.3, 512 MB RAM) | ≥ 60 FPS | Verificado |
| Campanha completa jogável do início ao fim (2 mundos, 10 ondas, 2 bosses) | 100% jogável | Concluído |
| Quiz com 48 perguntas sem repetição nas últimas 14 | 48 perguntas únicas | Concluído |
| Save/Load funcionando em 3 slots sem perda de dados | 0 falhas de save | Concluído |
| Testes automatizados passando (5 suites) | 100% aprovados | Concluído |
| Binário compilado abaixo de 5 MB | ~2,7 MB | Verificado |
| Sem coleta de dados pessoais (LGPD) | Zero network calls | Verificado |

---

## Métricas Educacionais (projetadas para uso em sala)

> Estas métricas dependem de aplicação real nas escolas — são metas para estudos futuros de impacto.

| Métrica | Meta esperada | Como medir |
| ------- | ------------- | ---------- |
| Taxa de acerto no quiz in-game | ≥ 60% de acerto médio por sessão | Log de respostas (futuro) |
| Engajamento por sessão | ≥ 20 min de jogo contínuo | Tempo de sessão (futuro) |
| Reconhecimento de doenças pós-jogo | Estudante identifica dengue, KPC e influenza após jogar | Questionário pré/pós (futuro) |
| Retenção de conteúdo vs. método tradicional | +20–40% vs. aula expositiva | Comparação em pesquisa controlada (futuro) |
| Adoção por professores do DF | ≥ 1 escola piloto no DF | Parceria com SEDF / SES-DF |

---

## Indicadores de Adoção

| Indicador | Meta inicial |
| --------- | ------------ |
| Downloads / clones do repositório GitHub | Monitorado por GitHub Insights |
| Issues e feedback abertos no repositório | Qualquer feedback de professor ou aluno |
| Uso em apresentação para a banca | Demonstração ao vivo — 18/06/2026 |
| Apresentação para a rede de ensino do DF | Meta futura pós-entrega |

---

## O que "sucesso mínimo" significa para este projeto

Para os fins do **Projeto Integrador**, o sucesso é atingido quando:

1. O jogo roda completamente do tutorial à tela de vitória, sem crashes
2. O conteúdo educativo está presente e funcionando (quiz, banners, nomes de doenças reais)
3. O código está documentado, versionado e com testes automatizados
4. A documentação de elicitação reflete fielmente o produto entregue
