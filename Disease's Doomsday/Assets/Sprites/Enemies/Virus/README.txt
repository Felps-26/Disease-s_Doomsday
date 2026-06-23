SPRITES DOS INIMIGOS — MUNDO 2 (VÍRUS)
=======================================

PNGs dos vírus (com escudo de capsídeo). Sem o arquivo, o inimigo é desenhado
proceduralmente (enemy_model.c) — cada tipo tem silhueta, tamanho, paleta e
animação PRÓPRIOS, então o jogo continua jogável e visualmente distinto sem PNGs.

O elenco viral tem CINCO identidades distintas (ver src/entities/enemy.c para os
arquétipos centralizados: HP, velocidade, capsídeo, tamanho, comportamento).

Arquivos esperados (nomes EXATOS):
  - swarm.png   -> vírus de ENXAME (tema rinovírus): pequeno, rápido, frágil,
                   capsídeo fraco, aparece em grande número.
  - melee.png   -> vírus ENVELOPADO corpo a corpo (tema dengue): médio, investida,
                   dano de contato, envelope com espículas.
  - ranged.png  -> vírus ATIRADOR (tema influenza): mantém distância, dispara
                   material viral e reposiciona; espículas HA/NA.
  - elite.png   -> vírus ELITE/MUTANTE (tema sarampo): grande, capsídeo reforçado,
                   alterna entre perseguir e atirar; como mini chefe, invoca enxame.
  - boss.png    -> CHEFE viral (tema coronavírus, onda 5): único e bem maior,
                   coroa de espículas; muda de aparência quando o capsídeo rompe.
  - shield.png  -> OVERLAY do escudo de capsídeo (desenhado por cima do vírus
                   enquanto o escudo estiver ativo).

Recomendações:
  - Dimensão sugerida: 64 x 64 px (swarm); 96 x 96 px (melee/ranged);
    160 x 160 px (elite); 256 x 256 px (boss);
    o overlay shield.png deve ser um pouco maior que o corpo (ex.: 128 x 128).
  - Fundo TRANSPARENTE. Vista de cima.
  - NÃO reutilize o mesmo PNG para tipos diferentes (cada arquétipo é distinto).
