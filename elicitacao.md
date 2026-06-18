# 📄 Documento de Elicitação — Disease's Doomsday

> **Disciplina:** Projeto Integrador
> **Data:** Junho de 2026
> **Versão:** 1.0

---

## 1. Identificação do Produto

| Campo | Informação |
|---|---|
| **Nome do Produto** | Disease's Doomsday |
| **Tipo** | Jogo educativo digital (Desktop) |
| **Linguagem / Stack** | C + Raylib |
| **Plataforma-alvo** | Windows · Linux · macOS |
| **Setor** | Educação em Saúde Pública |
| **Tema** | Vigilância Epidemiológica e prevenção de doenças infecciosas |

---

## 2. Análise 5W2H

### 2.1 What — O quê?

**Disease's Doomsday** é um jogo de ação 2D *top-down* em que o jogador controla um **Anticorpo** injetado na corrente sanguínea de um paciente infectado. O objetivo é eliminar patógenos onda após onda, atravessando dois mundos temáticos (Bactérias e Vírus), aprendendo sobre doenças reais, formas de transmissão e medidas de prevenção ao longo da jornada.

**Funcionalidades principais:**
- 2 mundos com 5 ondas cada (total de 10 ondas + 2 chefes)
- Quiz da Vigilância Epidemiológica entre ondas
- Sistema de progressão (XP, nível, Pontos do SUS)
- 4 armas com temática biológica real
- Power-ups e drops educativos (Máscara Hospitalar, Distanciamento Social)
- Mapa do corpo humano com órgãos-alvo identificados
- Save/Load em 3 slots com preview

---

### 2.2 Why — Por quê?

**Problema identificado:** Conteúdos de vigilância epidemiológica e saúde pública são frequentemente percebidos como densos e distantes da realidade do estudante ou do público geral.

**Solução proposta:** Transformar esses conteúdos em mecânicas de jogo, gerando engajamento ativo e aprendizagem significativa por meio da experiência lúdica.

**Justificativas:**

- 📊 O Brasil é um dos países com maior incidência de dengue no mundo; o Distrito Federal registra surtos anuais.
- 🏥 A resistência antimicrobiana (superbactérias) é considerada pela OMS uma das 10 maiores ameaças à saúde global.
- 🎮 Estudos apontam que *serious games* aumentam a retenção de conteúdo educativo em até 40% em comparação com métodos tradicionais.
- 🧠 A faixa etária prioritária (adolescentes e jovens adultos) consome jogos digitais como mídia principal.

---

### 2.3 Who — Quem?

#### Público-Alvo

| Perfil | Descrição | Faixa Etária |
|---|---|---|
| **Estudantes de Ensino Médio** | Interessados em saúde, biologia e tecnologia | 14–18 anos |
| **Estudantes de Ensino Superior** | Áreas de saúde, educação e computação | 18–25 anos |
| **Professores e Educadores** | Potenciais multiplicadores do conteúdo | Adultos |
| **Público Geral Interessado** | Pessoas que buscam informações sobre saúde de forma acessível | 14–35 anos |

**Persona Principal — "Lucas"**
> Estudante do 2º ano do ensino médio, 16 anos, mora no DF, joga jogos digitais diariamente, tem dificuldade em absorver conteúdos extensos de ciências. Com o jogo, aprende sobre dengue e superbactérias enquanto se diverte.

---

### 2.4 When — Quando?

| Marco | Data Prevista |
|---|---|
| Entrega dos artefatos no GitHub/GitLab | Até 18/06/2026 |
| Apresentação para a banca | 18/06/2026 |
| Versão jogável (MVP) | ✅ Concluído |
| Expansão Mundo 2 (Vírus) | Em desenvolvimento |
| Pipeline de sprites finais | Em produção |

**Cronograma de Desenvolvimento (retrospectivo):**

```
Fase 0: Fundação (enum Mundo, save/load)          → Concluída
Fase 1: Pipeline de Sprites                        → Concluída
Fase 2: Mapa do Corpo Humano                       → Concluída
Fase 3: Mundo 1 — Bactérias (inimigos, armas)      → Concluída
Fase 4: Mundo 2 — Vírus (escudo capsídeo)          → Em andamento
Fase 5: Itens compartilhados                       → Em andamento
Fase 6: História educativa e fluxo entre mundos    → Em andamento
Fase 7: Polimento e verificação final              → Pendente
```

---

### 2.5 Where — Onde?

| Contexto | Descrição |
|---|---|
| **Ambiente de execução** | Desktop local (sem necessidade de internet para jogar) |
| **Distribuição** | GitHub / GitLab (código-fonte + executável) |
| **Uso previsto** | Sala de aula, laboratório de informática, uso doméstico |
| **Potencial futuro** | Servidor web (versão browser via Emscripten/WebAssembly) ou Cloud Gaming |
| **Repositório** | Felps-26/Disease-s_Doomsday |

---

### 2.6 How — Como?

**Tecnicamente:**

1. Desenvolvido em **C puro** com a biblioteca **Raylib** (código aberto, multiplataforma)
2. Arquitetura modular com separação clara de responsabilidades (entities, systems, render, logic)
3. Arte visual procedural com pipeline pronto para receber sprites PNG
4. Build multiplataforma via `compilar.bat` (Windows) e `Makefile` / `build.sh` (Linux/macOS)
5. Save/Load assíncrono em thread com retrocompatibilidade entre versões

**Pedagogicamente:**

1. Cada mecânica de jogo é mapeada para um conceito científico real
2. Quiz com feedback educativo entre ondas
3. Mensagens educativas integradas ao fluxo do jogo (banners, loading tips, cutscenes)
4. Tom informativo e acionável — sem alarmismo

---

### 2.7 How Much — Quanto custa?

> Detalhado na seção **5. Custo Computacional** abaixo.

---

## 3. Stakeholders

### 3.1 Mapa de Stakeholders

```
Disease's Doomsday
       │
       ├─── 🎓 ACADÊMICO
       │         ├─ Equipe de Desenvolvimento (alunos)
       │         ├─ Professor da Disciplina (orientador)
       │         ├─ Coordenador do Curso
       │         └─ Instituição de Ensino
       │
       ├─── 🏥 SAÚDE
       │         ├─ Representante do Setor de Saúde (banca)
       │         ├─ Secretaria de Saúde do DF (potencial parceiro)
       │         └─ Profissionais de Saúde Pública
       │
       └─── 👥 USUÁRIOS
                 ├─ Estudantes de Ensino Médio / Superior
                 ├─ Professores e Educadores
                 └─ Público Geral
```

### 3.2 Detalhamento dos Stakeholders

| Stakeholder | Papel | Interesse | Influência |
|---|---|---|---|
| **Equipe de Desenvolvimento** | Criadores e mantenedores do produto | Entregar um produto funcional, educativo e bem avaliado | Alta |
| **Professor da Disciplina** | Orientador e avaliador | Verificar a aplicação de conceitos de engenharia de software | Alta |
| **Professor de Publicidade** | Avaliador | Avaliar comunicação, design e apelo do produto | Média |
| **Coordenador do Curso** | Avaliador institucional | Verificar alinhamento com objetivos do curso | Média |
| **Representante do Setor de Saúde** | Avaliador técnico | Verificar correção e relevância do conteúdo de saúde | Alta |
| **Estudantes (usuários finais)** | Jogadores | Entretenimento e aprendizagem | Baixa (indireta) |
| **Professores de Ciências/Biologia** | Multiplicadores | Uso do jogo como ferramenta pedagógica | Média |
| **Secretaria de Saúde do DF** | Parceiro potencial | Divulgação e uso em campanhas de saúde pública | Baixa (potencial) |

---

## 4. Requisitos Levantados

### 4.1 Requisitos Funcionais

| ID | Requisito | Prioridade |
|---|---|---|
| RF01 | O jogo deve apresentar dois mundos temáticos (Bactérias e Vírus) | Alta |
| RF02 | Cada mundo deve conter 5 ondas progressivas com chefe na última | Alta |
| RF03 | O sistema de Quiz deve aparecer entre as ondas com feedback educativo | Alta |
| RF04 | O jogador deve poder salvar e carregar o progresso em 3 slots | Alta |
| RF05 | As armas devem refletir conceitos biológicos reais | Média |
| RF06 | O mapa deve representar o corpo humano com órgãos-alvo identificados | Média |
| RF07 | O jogo deve exibir mensagens educativas (banners, tips) | Alta |
| RF08 | O sistema de progressão (XP, nível, Pontos do SUS) deve funcionar entre ondas | Alta |
| RF09 | Os inimigos do Mundo 2 devem possuir escudo de capsídeo | Média |
| RF10 | O jogo deve funcionar sem conexão à internet | Alta |

### 4.2 Requisitos Não Funcionais

| ID | Requisito | Categoria |
|---|---|---|
| RNF01 | O jogo deve rodar a 60 FPS em hardware com pelo menos 2GB de RAM | Desempenho |
| RNF02 | O código deve ser modular, com separação clara de responsabilidades | Manutenibilidade |
| RNF03 | O build deve funcionar em Windows, Linux e macOS | Portabilidade |
| RNF04 | Todo o conteúdo educativo deve ser scientificamente correto | Qualidade |
| RNF05 | O jogo deve carregar em menos de 5 segundos em hardware padrão | Desempenho |
| RNF06 | O código-fonte deve ser versionado e documentado no GitHub/GitLab | Rastreabilidade |
| RNF07 | A interface deve ser intuitiva, sem necessidade de manual para iniciar | Usabilidade |

---

## 5. Custo Computacional

### 5.1 Requisitos Mínimos de Hardware (Execução Local)

| Componente | Mínimo | Recomendado |
|---|---|---|
| **CPU** | Dual-core 1,5 GHz | Quad-core 2,0 GHz+ |
| **RAM** | 512 MB livres | 1 GB+ |
| **GPU** | Integrada (OpenGL 3.3) | Dedicada 512 MB VRAM |
| **Armazenamento** | ~50 MB (executável + assets) | ~100 MB (com sprites PNG) |
| **OS** | Windows 7+, Ubuntu 18+, macOS 10.14+ | Versões recentes |
| **Resolução** | 800×600 (janela redimensionável) | 1280×720 ou superior |

### 5.2 Custo de Build (Desenvolvimento)

| Recurso | Valor |
|---|---|
| **Ferramentas** | GCC / MinGW (gratuito) · Raylib (gratuito, open-source) |
| **IDE** | Qualquer editor de texto (VS Code, Vim, etc.) |
| **Tempo de compilação** | ~5–15 segundos (incremental < 3 s) |
| **Tamanho do binário** | ~2,7 MB (Windows) |

### 5.3 Custo Operacional — Distribuição Local

Para uso em laboratório de informática (sem servidor):

| Item | Estimativa |
|---|---|
| Preparação do ambiente por máquina | ~10 minutos (instalação Raylib + executável) |
| Espaço em disco por máquina | ~100 MB |
| Necessidade de internet durante uso | Nenhuma |
| Custo de licenciamento | **R$ 0,00** (open-source) |

### 5.4 Plano de Instanciamento em Cloud (Cenário Futuro)

Caso o jogo seja portado para **WebAssembly (Emscripten)** e hospedado em servidor web:

#### Opção A — Hospedagem Estática (Recomendada para MVP Web)

| Provedor | Serviço | Custo Estimado |
|---|---|---|
| **GitHub Pages** | Estático gratuito | R$ 0,00/mês |
| **Netlify Free** | CDN + deploy automático | R$ 0,00/mês (até 100 GB/mês) |
| **Cloudflare Pages** | CDN global gratuito | R$ 0,00/mês |

> Viável para distribuição educacional sem custo. O jogo roda 100% no navegador do cliente (sem processamento server-side).

#### Opção B — Servidor Dedicado (Para Futuras Features Multiplayer / Ranking)

| Componente | Especificação | Custo Estimado (mensal) |
|---|---|---|
| **Compute** | 1 vCPU, 1 GB RAM (ex: AWS t3.micro) | ~R$ 25–50/mês |
| **Banco de Dados** | PostgreSQL gerenciado (rankings/usuários) | ~R$ 30–80/mês |
| **CDN / Storage** | Assets estáticos (S3 + CloudFront) | ~R$ 5–20/mês |
| **Total estimado** | — | **~R$ 60–150/mês** |

#### Opção C — Plataforma PaaS (Heroku / Railway — Simplicidade Máxima)

| Serviço | Plano | Custo |
|---|---|---|
| **Railway** | Starter (512 MB RAM) | ~R$ 25/mês |
| **Render** | Instância básica | R$ 0–35/mês |

#### Configuração de Segurança da Informação (Servidor Web)

Caso o jogo seja alocado em servidor cliente, as seguintes medidas devem ser adotadas:

| Medida | Implementação |
|---|---|
| **HTTPS obrigatório** | Certificado SSL/TLS via Let's Encrypt (gratuito) |
| **Headers de segurança** | CSP, X-Frame-Options, HSTS configurados no servidor |
| **Sem dados sensíveis** | O jogo não coleta dados pessoais (saves são locais no browser via localStorage) |
| **Rate limiting** | Nginx/Cloudflare com limite de requisições por IP |
| **Atualizações** | Dependências revisadas a cada release (Raylib é auditável — C puro) |
| **Backups** | Snapshot semanal do servidor (caso haja ranking em banco) |
| **Isolamento** | Contêiner Docker para isolamento do processo do jogo |

---

## 6. Plano de UX/UI — Design System

### 6.1 Princípios de Design

O jogo segue os princípios do **Material Design** adaptados para jogos:

- **Clareza**: HUD limpo com informações essenciais sempre visíveis
- **Feedback imediato**: Números de dano flutuantes, partículas, screen shake
- **Consistência**: Paleta de cores e iconografia coerentes em todas as telas
- **Acessibilidade**: Controles simples, texto legível, sem dependência de cores para informações críticas

### 6.2 Paleta de Cores (Temática Biológica)

| Token | Cor | Uso |
|---|---|---|
| `COLOR_BACKGROUND` | `#0A0A1A` (azul-noite) | Fundo do jogo |
| `COLOR_PLAYER` | `#00BFFF` (ciano bioluminescente) | Anticorpo (herói) |
| `COLOR_ENEMY_BACTERIA` | `#FF4444` (vermelho) | Inimigos bacterianos |
| `COLOR_ENEMY_VIRUS` | `#AA44FF` (violeta) | Inimigos virais |
| `COLOR_HEALTH` | `#44FF44` (verde) | Barra de vida |
| `COLOR_SHIELD` | `#4488FF` (azul) | Escudo/capsídeo |
| `COLOR_XP` | `#FFD700` (dourado) | XP e pontos |
| `COLOR_BOSS` | `#FF8800` (laranja) | Boss e alertas |
| `COLOR_UI` | `#FFFFFF` com opacidade | Textos e ícones |

### 6.3 Tipografia

| Uso | Fonte | Tamanho |
|---|---|---|
| Títulos e banners | Fonte bitmap personalizada (Raylib) | 24–48 px |
| HUD (HP, XP) | Fonte bitmap | 16–20 px |
| Diálogos e textos educativos | Fonte bitmap | 14–18 px |
| Hotbar (teclas) | Fonte bitmap | 12–14 px |

### 6.4 Fluxo de Telas (User Flow)

```
[Tela de Boot / Splash]
         │
         ▼
[Menu Principal]
    ├── [JOGAR] ──► [Tutorial Interativo] ──► [Gameplay - Mundo 1]
    ├── [CARREGAR] ──► [Seleção de Slot] ──► [Gameplay]
    ├── [ARSENAL] ──► [Detalhes de Armas]
    ├── [SKINS] ──► [Preview ao vivo]
    ├── [TUTORIAL] ──► [Guia em abas]
    ├── [CONFIG] ──► [Volume / Skins]
    └── [SAIR]

[Gameplay]
    ├── [Entre ondas] ──► [Quiz] ──► [Melhorias do SUS]
    ├── [Derrota] ──► [Game Over] ──► [Menu / Tentar Novamente]
    ├── [Fim do Mundo 1] ──► [Cutscene Educativa] ──► [Mundo 2]
    └── [Fim do Mundo 2] ──► [Tela de Vitória]
```

### 6.5 HUD (Heads-Up Display)

```
┌─────────────────────────────────────────────┐
│ [Vida: ████████░░] [XP: ██░░░░] Nível: 3    │
│ Órgão: Pulmões | Doença: Pneumonia Bacteriana│
│                                             │
│          [Área de jogo]                     │
│                                             │
│ [Radar]              [Onda: 3/5] [Inimigos: 7]│
│ [1:Espada] [2:Rifle] [3:Granada] [4:Vacina]  │
└─────────────────────────────────────────────┘
```

---

## 7. Plano de Segurança da Informação

### 7.1 Análise de Risco (Execução Local)

| Risco | Probabilidade | Impacto | Mitigação |
|---|---|---|---|
| Corrupção de arquivo de save | Baixa | Médio | Validação na leitura + save em múltiplos slots |
| Modificação de executável | Baixa | Baixo | Distribuição via repositório verificado |
| Uso não autorizado do código | Média | Baixo | Licença open-source com créditos obrigatórios |

### 7.2 Análise de Risco (Cenário Servidor Web)

| Risco | Probabilidade | Impacto | Mitigação |
|---|---|---|---|
| DDoS | Média | Alto | Cloudflare WAF + rate limiting |
| Injeção em ranking/banco | Baixa | Alto | Prepared statements + sanitização de inputs |
| Coleta indevida de dados | Baixa | Alto | Política de privacidade clara; sem coleta de PII |
| Vulnerabilidade no servidor | Média | Alto | Atualizações automáticas + monitoramento |
| MITM (interceptação) | Baixa | Médio | HTTPS obrigatório |

### 7.3 Política de Dados

- **Dados coletados localmente**: progresso de jogo salvo em arquivos `.txt` simples no diretório `Saves/`
- **Dados pessoais**: **nenhum dado pessoal é coletado** na versão atual
- **Em cenário web**: autenticação mínima (opcional), apenas dados de ranking (apelido + pontuação)
- **LGPD**: em conformidade — sem coleta de PII, sem cookies de rastreamento

---

## 8. Planejamento de Product Management

### 8.1 Metodologia

O projeto foi desenvolvido com abordagem **iterativa incremental**, usando fases bem definidas (Fase 0 a 7) inspiradas em sprints ágeis.

### 8.2 Backlog de Produto

| Prioridade | Item | Status |
|---|---|---|
| 🔴 Alta | MVP jogável (Mundo 1 completo) | ✅ Concluído |
| 🔴 Alta | Sistema de Quiz educativo | ✅ Concluído |
| 🔴 Alta | Save/Load multi-slot | ✅ Concluído |
| 🟡 Média | Mundo 2 — Vírus com capsídeo | 🔄 Em andamento |
| 🟡 Média | Mapa do corpo humano | ✅ Concluído |
| 🟡 Média | Pipeline de sprites | ✅ Concluído |
| 🟢 Baixa | Sprites PNG finais | ⏳ Em produção |
| 🟢 Baixa | Versão Web (WebAssembly) | 📋 Planejado |
| 🟢 Baixa | Ranking online | 📋 Planejado |

### 8.3 Definition of Done

Um item é considerado **concluído** quando:
1. Compila sem novos warnings
2. Roda sem crashes
3. Save/Load funciona com retrocompatibilidade
4. Conteúdo educativo revisado pela equipe
5. Documentado no `README.md`

---

## 9. Referências

- OMS — *Antimicrobial Resistance: Global Report on Surveillance* (2023)
- Ministério da Saúde do Brasil — *Boletim Epidemiológico de Dengue* (2025)
- Raylib — https://www.raylib.com/ (licença zlib — open-source)
- Prensky, M. — *Digital Game-Based Learning* (2001)
- SES-DF — *Informe Epidemiológico Semanal* (2025)
