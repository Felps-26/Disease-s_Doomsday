# 7.2 Análise de Risco (Cenário Servidor Web)

| Risco                       | Probabilidade | Impacto | Mitigação                                        |
| --------------------------- | ------------- | ------- | ------------------------------------------------ |
| DDoS                        | Média         | Alto    | Cloudflare WAF + rate limiting                   |
| Injeção em ranking/banco    | Baixa         | Alto    | Prepared statements + sanitização de inputs      |
| Coleta indevida de dados    | Baixa         | Alto    | Política de privacidade clara; sem coleta de PII |
| Vulnerabilidade no servidor | Média         | Alto    | Atualizações automáticas + monitoramento         |
| MITM (interceptação)        | Baixa         | Médio   | HTTPS obrigatório                                |
