SPRITES DOS INIMIGOS — MUNDO 2 (VÍRUS)
=======================================

PNGs dos vírus (com escudo de capsídeo). Sem o arquivo, o inimigo é desenhado
proceduralmente (enemy_model.c).

Arquivos esperados (nomes EXATOS):
  - melee.png   -> vírus corpo a corpo com escudo (tema dengue)
  - ranged.png  -> vírus à distância com escudo (tema influenza)
  - boss.png    -> chefe viral com escudo reforçado (onda 5)
  - shield.png  -> OVERLAY do escudo de capsídeo (desenhado por cima do vírus
                   enquanto o escudo estiver ativo)

Recomendações:
  - Dimensão sugerida: 96 x 96 px (melee/ranged); 256 x 256 px (boss);
    o overlay shield.png deve ser um pouco maior que o corpo (ex.: 128 x 128).
  - Fundo TRANSPARENTE. Vista de cima.
