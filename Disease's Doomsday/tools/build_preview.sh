#!/bin/zsh
# Dev helper: compila uma ferramenta de preview offline ligando o jogo inteiro
# (exceto src/main.c). Uso:  tools/build_preview.sh <fonte.c> <saida>
# Ex.:  tools/build_preview.sh tools/ui_preview.c ui_preview && ./ui_preview
set -e
cd "${0:a:h}/.."
SRC="$1"; OUT="$2"
cc -O2 -Wall -Wno-missing-braces -Iinclude -I/opt/homebrew/include -I/usr/local/include \
  "$SRC" \
  src/logic/update_gameplay.c src/logic/fsm.c src/gameplay/input_controller.c src/gameplay/spatial_grid.c \
  src/render/render_gameplay.c src/entities/player.c src/entities/enemy.c src/entities/projectiles.c \
  src/systems/combat_system.c src/systems/wave_manager.c src/telas/telas.c src/telas/tela_quiz.c \
  src/telas/tela_upgrade.c src/telas/tela_arsenal.c src/telas/tela_skins.c src/telas/tela_admin.c \
  src/assets/asset_manager.c src/assets/sprite_manager.c \
  Assets/@models/player_model.c Assets/@models/enemy_model.c Assets/@models/weapons_model.c \
  Assets/@models/doctor_model.c Assets/@models/cosmetics.c Assets/Maps/map_seringa.c Assets/Maps/map_body.c \
  -o "$OUT" -L/opt/homebrew/lib -L/usr/local/lib -lraylib -framework OpenGL -framework Cocoa \
  -framework IOKit -framework CoreAudio -framework CoreVideo -lm
echo "[build_preview] -> ./$OUT"
