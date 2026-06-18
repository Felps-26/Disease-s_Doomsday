SPRITES DO PLAYER (Anticorpo)
=============================

Coloque aqui os PNGs do herói (Anticorpo), um por skin. Enquanto o arquivo
não existir, o jogo desenha o herói proceduralmente (player_model.c) — nada
quebra.

Arquivos esperados (nomes EXATOS):
  - anticorpo_default.png     -> skin 0 (Padrão)
  - anticorpo_medica.png      -> skin 1 (Médica)
  - anticorpo_infectada.png   -> skin 2 (Infectada)

Recomendações:
  - Dimensão sugerida: 128 x 128 px (quadrado). É desenhado centralizado,
    escalado para ~64 px no mundo.
  - Fundo TRANSPARENTE (PNG com canal alfa).
  - Vista de cima (top-down). O herói "olha" para a direita por padrão.

Futuro (animação por skin): se forem usadas folhas idle/move/attack, manter o
mesmo prefixo (ex.: anticorpo_default_idle.png) — a pipeline será estendida.
