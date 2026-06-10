# Void Venture - RPG em C com Raylib

## Sobre o projeto

Void Venture é um jogo 2D desenvolvido em linguagem C com a biblioteca Raylib. O projeto foi criado como um RPG de sobrevivência em arena, com movimentação livre pelo mapa, combate em tempo real, inimigos por ondas, sistema de progressão, power-ups, partículas, projéteis, seleção de skins e sistema de salvamento.

O jogo utiliza uma estrutura modular, separando a lógica principal, as telas, o gameplay e as definições globais em arquivos diferentes. Isso facilita a organização do código e torna o projeto mais claro para estudo, manutenção e apresentação acadêmica.

---

## Objetivo do jogo

O objetivo do jogador é sobreviver às ondas de inimigos, derrotar criaturas cada vez mais fortes, coletar power-ups e evoluir o personagem até alcançar a vitória.

Durante a partida, o jogador ganha pontos e experiência ao eliminar inimigos. Ao acumular experiência suficiente, o personagem sobe de nível, aumentando seus atributos de vida, ataque e velocidade.

---

## Principais funcionalidades

- Jogo 2D em tempo real.
- Movimentação livre pelo mapa.
- Sistema de combate corpo a corpo.
- Ataque por teclado ou mouse.
- Inimigos com diferentes tipos e comportamentos.
- Inimigos à distância com projéteis.
- Sistema de ondas.
- Boss em ondas avançadas.
- Sistema de level up.
- Power-ups com efeitos temporários.
- Sistema de partículas.
- Efeito de screen shake.
- Câmera suave seguindo o jogador.
- Menu principal.
- Tela de controles.
- Tela de pausa.
- Tela de game over.
- Tela de vitória.
- Seleção de skins.
- Sistema de save/load com slots.
- Quicksave e quickload.
- Música de fundo com sistema de crossfade.
- Suporte a janela redimensionável com letterbox.

---

## Tecnologias utilizadas

- Linguagem C
- Raylib
- GCC
- Windows Batch Script
- Manipulação de arquivos
- Estruturas
- Enums
- Vetores
- Arrays
- Ponteiros
- Modularização em C

---

## Estrutura do projeto

text JOGO_JOGÁVEL3/ │ ├── Assets/ │   ├── Musica/ │   ├── Sprites/ │   └── font.ttf │ ├── Gameplay/ │   ├── gameplay.c │   └── gameplay.h │ ├── Saves/ │ ├── Telas/ │   ├── telas.c │   └── telas.h │ ├── compilar.bat ├── game.h └── rpg.c 

---

## Descrição dos arquivos e pastas

### rpg.c

Arquivo principal do jogo.

É responsável por:

- Inicializar a janela do jogo.
- Configurar resolução, VSync e FPS.
- Carregar fonte, músicas e texturas.
- Inicializar o estado global do jogo.
- Controlar o loop principal.
- Atualizar a tela atual.
- Renderizar o jogo.
- Controlar o sistema de save/load rápido.
- Finalizar corretamente áudio, texturas e janela.

Também é nesse arquivo que ocorre o controle geral das telas, alternando entre menu, gameplay, pausa, seleção de saves, game over e vitória.

---

### game.h

Arquivo de definições globais do projeto.

Contém:

- Constantes do jogo.
- Tamanho da tela.
- Tamanho do mapa.
- Limites máximos de inimigos, power-ups, partículas e projéteis.
- Enumerações das telas.
- Enumerações dos estados dos inimigos.
- Enumerações dos tipos de power-up.
- Estruturas principais do jogo.

As principais estruturas declaradas são:

- Player
- Enemy
- PowerUp
- Particle
- Projectile
- UIButton
- SaveSlotMeta
- GameState

A estrutura GameState centraliza o estado completo do jogo, incluindo jogador, inimigos, power-ups, partículas, projéteis, câmera, tela atual, slots de save e texturas carregadas.

---

### Gameplay/gameplay.c

Arquivo responsável pela lógica principal da partida.

Implementa:

- Inicialização do jogo.
- Criação de ondas de inimigos.
- Movimentação do jogador.
- Ataque do jogador.
- Inteligência artificial dos inimigos.
- Patrulha dos inimigos.
- Perseguição ao jogador.
- Ataques corpo a corpo.
- Ataques à distância.
- Criação de projéteis.
- Colisão entre projéteis e jogador.
- Coleta de power-ups.
- Sistema de experiência.
- Sistema de level up.
- Sistema de partículas.
- Câmera suave.
- Screen shake.
- Sistema de salvamento.
- Sistema de carregamento.

Esse arquivo concentra a maior parte da lógica de gameplay.

---

### Gameplay/gameplay.h

Arquivo de cabeçalho do módulo de gameplay.

Declara as funções usadas pela lógica do jogo, permitindo que outros arquivos chamem funções como inicialização da partida, atualização da gameplay, salvamento e carregamento.

---

### Telas/telas.c

Arquivo responsável pela interface gráfica e pelas telas do jogo.

Implementa a renderização e atualização de telas como:

- Menu principal.
- Tela de controles.
- Tela de seleção de skins.
- Tela de gameplay.
- HUD.
- Tela de pausa.
- Tela de save.
- Tela de load.
- Tela de game over.
- Tela de vitória.

Também contém a lógica dos botões da interface.

---

### Telas/telas.h

Arquivo de cabeçalho do módulo de telas.

Declara as funções relacionadas à interface e à renderização, permitindo que rpg.c utilize as telas de forma modular.

---

### Assets/

Pasta que armazena os recursos visuais e sonoros do jogo.

Inclui:

- Sprites do personagem.
- Sprites dos inimigos.
- Sprites dos projéteis.
- Música de fundo.
- Fonte personalizada.

---

### Saves/

Pasta usada para armazenar os arquivos de salvamento do jogo.

O sistema salva informações como:

- Nome do jogador.
- Nível.
- Pontuação.
- Onda atual.
- Data do save.
- Posição do jogador.
- Vida.
- Experiência.
- Skin ativa.
- Estado geral da partida.

Também são salvas imagens de preview dos slots.

---

### compilar.bat

Script de compilação para Windows.

Ele usa o GCC incluído no ambiente da Raylib para compilar os arquivos principais do projeto:

text rpg.c Gameplay/gameplay.c Telas/telas.c 

O executável gerado é:

text rpg_game.exe 

---

## Controles

| Ação | Tecla |
|---|---|
| Mover para cima | W ou Seta para cima |
| Mover para baixo | S ou Seta para baixo |
| Mover para esquerda | A ou Seta para esquerda |
| Mover para direita | D ou Seta para direita |
| Atacar | Espaço ou botão esquerdo do mouse |
| Pausar / voltar | Esc |
| Salvar rapidamente | F5 |
| Carregar rapidamente | F9 |

---

## Power-ups

O jogo possui quatro tipos principais de power-ups:

| Power-up | Efeito |
|---|---|
| Recuperação de HP | Recupera parte da vida do jogador |
| Speed Boost | Aumenta temporariamente a velocidade |
| Shield | Protege temporariamente contra dano |
| Attack Boost | Aumenta temporariamente o dano do jogador |

Os power-ups aparecem pelo mapa e também podem surgir ao derrotar inimigos.

---

## Sistema de inimigos

Os inimigos são divididos em tipos e níveis de dificuldade.

### Tipos de inimigos

- Comum: patrulha o mapa e persegue o jogador quando se aproxima.
- Rápido: possui maior velocidade e pode atacar à distância.
- Elite: possui mais vida, causa mais dano e representa maior ameaça.
- Boss: aparece em ondas avançadas e possui grande quantidade de vida.

### Estados dos inimigos

Os inimigos usam uma máquina de estados simples:

- IDLE: parado ou patrulhando.
- AGGRO: perseguindo o jogador.
- ATTACK: executando ataque.
- HURT: sofrendo dano ou recuo.
- DEATH: derrotado.

---

## Sistema de ondas

A partida é organizada em ondas.

A cada onda:

- A quantidade de inimigos aumenta.
- Os inimigos ficam mais fortes.
- Novos power-ups são espalhados pelo mapa.
- O jogador precisa eliminar todos os inimigos para avançar.

Ao superar as ondas finais, o jogo exibe a tela de vitória.

---

## Sistema de level up

O jogador ganha experiência ao derrotar inimigos.

Quando a experiência acumulada atinge o valor necessário, o jogador sobe de nível.

Ao subir de nível:

- A vida máxima aumenta.
- A vida é restaurada.
- O poder de ataque aumenta.
- A velocidade base aumenta.
- Um efeito visual de partículas é exibido.

---

## Sistema de save/load

O jogo possui sistema de salvamento por slots.

É possível salvar e carregar partidas usando as telas do jogo ou os atalhos:

text F5 = salvar rapidamente no slot 1 F9 = carregar rapidamente do slot 1 

Cada slot armazena metadados da partida, incluindo:

- Nome do jogador.
- Nível.
- Pontuação.
- Onda atual.
- Data e horário do save.

Além disso, o jogo exporta uma imagem de preview do momento salvo.

---

## Como compilar

### Requisitos

Antes de compilar, é necessário ter:

- Windows.
- Raylib instalada.
- GCC disponível no ambiente da Raylib.
- Estrutura de pastas original preservada.

O script compilar.bat espera que a Raylib esteja instalada neste caminho:

text C:\raylib\ 

### Compilação

Abra o terminal dentro da pasta JOGO_JOGÁVEL3 e execute:

bat compilar.bat 

Se a compilação funcionar corretamente, será gerado o arquivo:

text rpg_game.exe 

Para executar o jogo:

bat rpg_game.exe 

---

## Comando de compilação usado

O script utiliza uma compilação equivalente a:

bat gcc rpg.c Gameplay\gameplay.c Telas\telas.c -o rpg_game.exe -O2 -Wall -Wno-missing-braces -I "C:\raylib\raylib\src" -L "C:\raylib\raylib\src" -lraylib -lopengl32 -lgdi32 -lwinmm -lm 

---

## Conceitos de programação aplicados

Este projeto utiliza diversos conceitos importantes da linguagem C e da programação de jogos:

- Modularização.
- Structs.
- Enums.
- Arrays.
- Ponteiros.
- Manipulação de arquivos.
- Estados de jogo.
- Sistema de telas.
- Loop principal de jogo.
- Entrada de teclado e mouse.
- Colisão.
- Renderização 2D.
- Câmera.
- Sistema de partículas.
- Persistência de dados.
- Organização de assets.

---

## Destaques técnicos

### Estado global centralizado

O jogo usa a estrutura GameState para armazenar o estado completo da partida. Isso facilita a passagem de dados entre módulos e permite que funções diferentes manipulem o mesmo estado do jogo.

### Separação por módulos

O projeto separa responsabilidades em arquivos diferentes:

- rpg.c: inicialização e loop principal.
- game.h: definições globais.
- gameplay.c: lógica da partida.
- telas.c: interface e renderização.

Essa separação deixa o código mais organizado e mais fácil de apresentar.

### Sistema de save

O sistema de save mostra uso prático de manipulação de arquivos em C, salvando dados da partida em arquivos .txt.

### Uso da Raylib

A Raylib é usada para criar janela, desenhar sprites, tocar música, capturar entrada do jogador, controlar câmera, renderizar texturas e gerenciar áudio.

---

## Possíveis melhorias futuras

Algumas melhorias que poderiam ser adicionadas ao projeto:

- Sistema de inventário.
- Mais tipos de inimigos.
- Mais fases ou mapas.
- Barra de vida para boss.
- Menu de configurações.
- Controle de volume.
- Sons de ataque e dano.
- Sistema de armas diferentes.
- Animações de morte mais completas.
- Suporte a Linux via Makefile.
- Balanceamento de dificuldade.
- Salvamento em formato binário ou JSON.
- Organização mais avançada dos assets.

---

## Créditos

Projeto desenvolvido em linguagem C com Raylib como jogo acadêmico para estudo de programação, modularização, ponteiros, estruturas, manipulação de arquivos e desenvolvimento de jogos 2D.

---

## Licença

Este projeto foi desenvolvido para fins educacionais. Caso seja utilizado, modificado ou distribuído, recomenda-se manter os créditos dos autores originais e da biblioteca Raylib.
