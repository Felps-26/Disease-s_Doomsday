// GERADO por tools/extract_organisms.py — NÃO editar à mão.
#ifndef MENU_ORGANISMS_H
#define MENU_ORGANISMS_H

#define MENU_ORGANISM_COUNT 18

typedef struct MenuOrganismDef { const char *path; int w, h; const char *type; const char *color; } MenuOrganismDef;

static const MenuOrganismDef MENU_ORGANISMS[MENU_ORGANISM_COUNT] = {
    { "Assets/Sprites/UI/Menu/Organisms/organism_00.png", 271, 267, "virus", "green" },
    { "Assets/Sprites/UI/Menu/Organisms/organism_01.png", 205, 212, "virus", "blue" },
    { "Assets/Sprites/UI/Menu/Organisms/organism_02.png", 227, 228, "virus", "blue" },
    { "Assets/Sprites/UI/Menu/Organisms/organism_03.png", 221, 218, "virus", "blue" },
    { "Assets/Sprites/UI/Menu/Organisms/organism_04.png", 179, 178, "virus", "green" },
    { "Assets/Sprites/UI/Menu/Organisms/organism_05.png", 145, 148, "virus", "blue" },
    { "Assets/Sprites/UI/Menu/Organisms/organism_06.png", 148, 146, "virus", "blue" },
    { "Assets/Sprites/UI/Menu/Organisms/organism_07.png", 219, 161, "virus", "green" },
    { "Assets/Sprites/UI/Menu/Organisms/organism_08.png", 232, 200, "virus", "blue" },
    { "Assets/Sprites/UI/Menu/Organisms/organism_09.png", 179, 184, "virus", "blue" },
    { "Assets/Sprites/UI/Menu/Organisms/organism_10.png", 264, 247, "virus", "green" },
    { "Assets/Sprites/UI/Menu/Organisms/organism_11.png", 291, 176, "bacteria", "blue" },
    { "Assets/Sprites/UI/Menu/Organisms/organism_12.png", 256, 207, "virus", "blue" },
    { "Assets/Sprites/UI/Menu/Organisms/organism_13.png", 242, 218, "virus", "green" },
    { "Assets/Sprites/UI/Menu/Organisms/organism_14.png", 305, 183, "bacteria", "blue" },
    { "Assets/Sprites/UI/Menu/Organisms/organism_15.png", 233, 198, "virus", "blue" },
    { "Assets/Sprites/UI/Menu/Organisms/organism_16.png", 182, 166, "virus", "green" },
    { "Assets/Sprites/UI/Menu/Organisms/organism_17.png", 142, 140, "virus", "blue" },
};

#endif
