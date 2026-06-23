# 5.4 Plano de Instanciamento em Cloud (Cenário Futuro)

Caso o jogo seja portado para **WebAssembly (Emscripten)** e hospedado em servidor web:

## Opção A — Hospedagem Estática (Recomendada para MVP Web)

| Provedor             | Serviço                 | Custo Estimado               |
| -------------------- | ----------------------- | ---------------------------- |
| **GitHub Pages**     | Estático gratuito       | R$ 0,00/mês                  |
| **Netlify Free**     | CDN + deploy automático | R$ 0,00/mês (até 100 GB/mês) |
| **Cloudflare Pages** | CDN global gratuito     | R$ 0,00/mês                  |

> Viável para distribuição educacional sem custo. O jogo roda 100% no navegador do cliente (sem processamento server-side).

## Opção B — Servidor Dedicado (Para Futuras Features Multiplayer / Ranking)

| Componente         | Especificação                             | Custo Estimado (mensal) |
| ------------------ | ----------------------------------------- | ----------------------- |
| **Compute**        | 1 vCPU, 1 GB RAM (ex: AWS t3.micro)       | ~R$ 25–50/mês           |
| **Banco de Dados** | PostgreSQL gerenciado (rankings/usuários) | ~R$ 30–80/mês           |
| **CDN / Storage**  | Assets estáticos (S3 + CloudFront)        | ~R$ 5–20/mês            |
| **Total estimado** | —                                         | **~R$ 60–150/mês**      |

## Opção C — Plataforma PaaS (Heroku / Railway — Simplicidade Máxima)

| Serviço     | Plano                | Custo       |
| ----------- | -------------------- | ----------- |
| **Railway** | Starter (512 MB RAM) | ~R$ 25/mês  |
| **Render**  | Instância básica     | R$ 0–35/mês |

## Configuração de Segurança da Informação (Servidor Web)

Caso o jogo seja alocado em servidor cliente, as seguintes medidas devem ser adotadas:

| Medida                   | Implementação                                                                   |
| ------------------------ | ------------------------------------------------------------------------------- |
| **HTTPS obrigatório**    | Certificado SSL/TLS via Let's Encrypt (gratuito)                                |
| **Headers de segurança** | CSP, X-Frame-Options, HSTS configurados no servidor                             |
| **Sem dados sensíveis**  | O jogo não coleta dados pessoais (saves são locais no browser via localStorage) |
| **Rate limiting**        | Nginx/Cloudflare com limite de requisições por IP                               |
| **Atualizações**         | Dependências revisadas a cada release (Raylib é auditável — C puro)             |
| **Backups**              | Snapshot semanal do servidor (caso haja ranking em banco)                       |
| **Isolamento**           | Contêiner Docker para isolamento do processo do jogo                            |
