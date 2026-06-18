@echo off
:: compilar.bat — Compila o RPG do JOGO_JOGÁVEL3 com os módulos do projeto

:: Configurações do Raylib e GCC
set "GCC=C:\raylib\w64devkit\bin\gcc.exe"
set "INCLUDE=C:\raylib\raylib\src"
set "LIB=C:\raylib\raylib\src"
set "FLAGS=-O2 -Wall -Wno-missing-braces"
set "LIBS=-lraylib -lopengl32 -lgdi32 -lwinmm -lm"

:: Adiciona a pasta bin ao PATH para o assembler funcionar
set "PATH=C:\raylib\w64devkit\bin;%PATH%"

:: Lista de todos os arquivos .c do projeto
set "SOURCES=src\main.c src\logic\update_gameplay.c src\logic\fsm.c src\gameplay\input_controller.c src\gameplay\spatial_grid.c src\render\render_gameplay.c src\entities\player.c src\entities\enemy.c src\entities\projectiles.c src\systems\combat_system.c src\systems\wave_manager.c src\telas\telas.c src\telas\tela_quiz.c src\telas\tela_upgrade.c src\telas\tela_arsenal.c src\telas\tela_skins.c src\telas\tela_admin.c src\assets\asset_manager.c src\assets\sprite_manager.c Assets\@models\player_model.c Assets\@models\enemy_model.c Assets\@models\weapons_model.c Assets\Maps\map_seringa.c Assets\Maps\map_body.c"


echo [INFO] Compilando Legend of the Square RPG (modular)...
echo [INFO] Arquivos: %SOURCES%

"%GCC%" %SOURCES% -o rpg_game.exe %FLAGS% -I "%INCLUDE%" -I "include" -L "%LIB%" %LIBS%

if %errorlevel% equ 0 (
    echo [OK] Compilacao concluida com sucesso!
    echo [INFO] Para rodar: .\rpg_game.exe
) else (
    echo [ERRO] Falha na compilacao.
)
