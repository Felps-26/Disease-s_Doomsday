SPRITES DO MAPA (corpo humano)
==============================

PNGs do mapa do corpo humano. Sem o arquivo, o mapa (silhueta + órgãos) é
desenhado proceduralmente (map_body.c).

Arquivos esperados (nomes EXATOS):
  - corpo.png      -> arte completa do corpo (silhueta + órgãos, camada única)
  - silhueta.png   -> apenas a silhueta do corpo (camada de fundo)
  - orgaos.png     -> apenas os órgãos-alvo destacados (camada por cima)

Recomendações:
  - Dimensão sugerida: proporcional ao mundo (MAP_WIDTH x MAP_HEIGHT = 4000x4000).
    Pode ser menor e escalado (ex.: 1024 x 1024) mantendo a proporção.
  - Fundo TRANSPARENTE nas camadas órgãos/silhueta.
