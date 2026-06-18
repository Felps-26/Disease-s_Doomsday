#ifndef WEAPONS_MODEL_H
#define WEAPONS_MODEL_H

#include "raylib.h"

// Desenha a Espada Seringa (liquidColor varia conforme a skin da arma)
void DrawSyringeSword(Vector2 handPos, float size, float rotationDeg, Color liquidColor);

// Desenha o Escalpelizador Estático (arma melee anti-escudo do Mundo 2)
void DrawScalpel(Vector2 handPos, float size, float rotationDeg, Color primary, Color secondary);

// Define o Mundo atual para a aparência da arma melee (slot 1): Espada-Seringa
// (Mundo 1) ou Escalpelizador Estático (Mundo 2).
void SetWeaponModelWorld(int world);

// Desenha a arma SEGURADA conforme o tipo equipado (1=melee, 2=projétil,
// 3=granada, 4=BFG). As cores da skin (primary/secondary) valem para todas.
void DrawHeldWeapon(int weapon, Vector2 handPos, float size, float rotationDeg, Color primary, Color secondary);

#endif // WEAPONS_MODEL_H
