#ifndef WEAPONS_MODEL_H
#define WEAPONS_MODEL_H

#include "raylib.h"

// Desenha a Espada Seringa (liquidColor varia conforme a skin da arma)
void DrawSyringeSword(Vector2 handPos, float size, float rotationDeg, Color liquidColor);

// Desenha a arma SEGURADA conforme o tipo equipado (1=Lâmina, 2=Fuzil,
// 3=Granada, 4=BFG). As cores da skin (primary/secondary) valem para todas.
void DrawHeldWeapon(int weapon, Vector2 handPos, float size, float rotationDeg, Color primary, Color secondary);

#endif // WEAPONS_MODEL_H
