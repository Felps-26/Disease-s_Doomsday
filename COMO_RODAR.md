# 🎮 Como Rodar o Disease's Doomsday — Guia Completo

Este guia foi feito para ajudar qualquer pessoa, desde desenvolvedores até leigos, a preparar o ambiente, integrar a biblioteca **Raylib** ao **VS Code**, compilar e executar o jogo.

---

## 📌 1. O que deve ser instalado?

Para rodar e compilar o jogo, precisamos de 3 coisas principais no Windows:

### A. Raylib (com compilador GCC/MinGW)
A forma mais fácil de instalar tudo de uma vez é usando o instalador completo da Raylib:
1. Acesse o site oficial: [raylib.com](https://www.raylib.com/) ou baixe diretamente do repositório no GitHub.
2. Baixe a versão instaladora para Windows (**raylib_installer_vX.X.X.mingw.exe**).
3. Execute o instalador e mantenha o caminho padrão: `C:\raylib`.
   - *Por que?* O script de compilação do jogo espera encontrar o compilador em `C:\raylib\w64devkit` e os arquivos da Raylib em `C:\raylib\raylib\src`.

### B. Visual Studio Code (VS Code)
Se ainda não tiver o editor instalado:
1. Baixe e instale em: [code.visualstudio.com](https://code.visualstudio.com/)

### C. Extensão C/C++ no VS Code
Para que o VS Code entenda o código em C e dê sugestões inteligentes (auto-complete):
1. Abra o VS Code.
2. Vá no ícone de **Extensões** na barra lateral esquerda (ou aperte `Ctrl + Shift + X`).
3. Pesquise por **C/C++** (desenvolvida pela Microsoft) e clique em **Instalar**.

---

## 🛠️ 2. Como Integrar o Raylib ao VS Code?

Para evitar avisos de erro vermelhos embaixo de `#include "raylib.h"` no VS Code, configure o **IntelliSense**:

1. No VS Code, abra a pasta do jogo (`Game`).
2. Pressione `Ctrl + Shift + P` para abrir a paleta de comandos do VS Code.
3. Digite: `C/C++: Edit Configurations (JSON)` e selecione essa opção. Isso criará uma pasta `.vscode` com um arquivo `c_cpp_properties.json`.
4. Substitua o conteúdo desse arquivo pelo seguinte para ensinar o VS Code onde achar a Raylib:

```json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "C:/raylib/raylib/src",
                "C:/raylib/w64devkit/include"
            ],
            "defines": [
                "_DEBUG",
                "UNICODE",
                "_UNICODE"
            ],
            "compilerPath": "C:/raylib/w64devkit/bin/gcc.exe",
            "cStandard": "c11",
            "cppStandard": "c++17",
            "intelliSenseMode": "windows-gcc-x64"
        }
    ],
    "version": 4
}
```

---

## 🚀 3. Como Compilar e Jogar?

Temos o script de compilação automática pronto (`compilar.bat`). Escolha o método mais fácil para você:

### Método Rápido (Recomendado para Leigos)
Se você só quer rodar o jogo sem mexer em código:

1. **Abrir o Terminal na pasta do jogo:**
   - Abra a pasta do jogo no Windows Explorer.
   - Clique na barra de endereço no topo, digite `cmd` e pressione `Enter`. (Isso abrirá uma tela preta de terminal já na pasta correta).
2. **Compilar o jogo:**
   - No terminal que abriu, digite:
     ```bash
     compilar.bat
     ```
   - Pressione `Enter` e aguarde alguns segundos até ver a mensagem `[OK] Compilacao concluida com sucesso!`.
3. **Executar o jogo:**
   - No mesmo terminal, digite:
     ```bash
     rpg_game.exe
     ```
   - Ou simplesmente dê dois cliques no arquivo `rpg_game.exe` criado na pasta.

---

### Método integrado no VS Code (Para Desenvolvedores)
Se você quer programar e testar direto pelo VS Code pressionando apenas um botão:

1. Dentro da pasta `.vscode` (criada no passo 2), crie um arquivo chamado `tasks.json`.
2. Adicione a seguinte configuração para associar o atalho de build ao `compilar.bat`:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Compilar Jogo (compilar.bat)",
            "type": "shell",
            "command": ".\\compilar.bat",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": ["$gcc"]
        }
    ]
}
```

3. Agora, para compilar a qualquer momento, basta pressionar `Ctrl + Shift + B` no VS Code!
4. Para rodar, você pode abrir o terminal integrado (`Ctrl + '`) e digitar `.\rpg_game.exe`.

---

## 🐧🍎 Rodando no Linux ou macOS

No Windows o build é feito pelo `compilar.bat`. Em Linux/macOS use o **Makefile** incluído na pasta `Game`.

1. **Instale a Raylib:**
   - **Linux (Debian/Ubuntu):** `sudo apt install libraylib-dev` (ou compile a Raylib a partir do código-fonte).
   - **macOS (Homebrew):** `brew install raylib`
2. **Compile e jogue** (dentro da pasta `Game`):
   ```bash
   make            # gera o executável ./rpg_game
   ./rpg_game
   ```
   Ou use o atalho que compila e já executa:
   ```bash
   ./build.sh
   ```
   > Importante: rode o jogo a partir da pasta `Game`, pois ele carrega `Assets/` e grava em `Saves/` por caminhos relativos.

---

## 🎮 Controles Gerais do Jogo

Após iniciar o jogo, você poderá usar os seguintes controles no teclado:

- **Movimentação:** Teclas `W`, `A`, `S`, `D` ou as setas do teclado.
- **Ataque / Interação:**
  - Botão esquerdo do mouse para usar sua arma.
  - Teclas `1`, `2`, `3` para trocar de arma (quando habilitadas/adquiridas).
- **Usar Poção de Vida:** Tecla `E`.
- **Menu / Pausa:** Tecla `Esc`.
