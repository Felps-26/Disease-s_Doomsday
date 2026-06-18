# Makefile — Disease's Doomsday (Linux / macOS)
# Equivalente multiplataforma ao compilar.bat (Windows).
#
# Uso:
#   make           # compila -> ./rpg_game
#   make run       # compila e executa
#   make clean     # remove o executável
#
# Requer a Raylib instalada e detectável via pkg-config (Linux) ou Homebrew (macOS).
#   Linux:  sudo apt install libraylib-dev   (ou compile a Raylib do código-fonte)
#   macOS:  brew install raylib

CC      ?= gcc
TARGET   = rpg_game
CFLAGS   = -O2 -Wall -Wno-missing-braces -Iinclude

# Lista explícita de fontes (mesma do compilar.bat). Os arquivos
# telas_common.c / debug_patch.c são legado e NÃO entram no build.
SRCS = \
  src/main.c \
  src/logic/update_gameplay.c \
  src/logic/fsm.c \
  src/gameplay/input_controller.c \
  src/gameplay/spatial_grid.c \
  src/render/render_gameplay.c \
  src/entities/player.c \
  src/entities/enemy.c \
  src/entities/projectiles.c \
  src/systems/combat_system.c \
  src/systems/wave_manager.c \
  src/telas/telas.c \
  src/telas/tela_quiz.c \
  src/telas/tela_upgrade.c \
  src/telas/tela_arsenal.c \
  src/telas/tela_skins.c \
  src/telas/tela_admin.c \
  src/assets/asset_manager.c \
  src/assets/sprite_manager.c \
  Assets/@models/player_model.c \
  Assets/@models/enemy_model.c \
  Assets/@models/weapons_model.c \
  Assets/Maps/map_seringa.c \
  Assets/Maps/map_body.c

# ---------------------------------------------------------------------------
# Detecção de plataforma e flags de link da Raylib
# ---------------------------------------------------------------------------
UNAME_S := $(shell uname -s)

# Tenta usar pkg-config; se existir, ele resolve includes e libs corretamente.
PKG_RAYLIB := $(shell pkg-config --exists raylib 2>/dev/null && echo yes)

ifeq ($(PKG_RAYLIB),yes)
  CFLAGS += $(shell pkg-config --cflags raylib)
  LIBS    = $(shell pkg-config --libs raylib) -lm
else ifeq ($(UNAME_S),Darwin)
  # macOS sem pkg-config: usa frameworks do sistema
  CFLAGS += -I/opt/homebrew/include -I/usr/local/include
  LIBS = -L/opt/homebrew/lib -L/usr/local/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit \
         -framework CoreAudio -framework CoreVideo -lm
else
  # Linux sem pkg-config: link manual padrão da Raylib
  LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

# ---------------------------------------------------------------------------
.PHONY: all run clean
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(SRCS) -o $(TARGET) $(CFLAGS) $(LIBS)
	@echo "[OK] Compilacao concluida -> ./$(TARGET)"

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
	@echo "[OK] Limpeza concluida."
