#!/usr/bin/env bash
# build.sh — Atalho de compilação para Linux/macOS (Disease's Doomsday)
# Compila via Makefile e, se tudo der certo, executa o jogo.
set -e

cd "$(dirname "$0")"

echo "[INFO] Compilando Disease's Doomsday..."
make

echo "[INFO] Iniciando o jogo..."
./rpg_game
