// GERADO por tools/extract_menu_sprites.py — NÃO editar à mão.
// Layout dos glifos do título (posição original relativa ao canto
// superior-esquerdo do título). Usado para montar o título letra a letra.
#ifndef MENU_TITLE_GLYPHS_H
#define MENU_TITLE_GLYPHS_H

#define MENU_TITLE_GLYPH_COUNT 17
#define MENU_TITLE_W 511
#define MENU_TITLE_H 181

typedef struct MenuGlyphDef { const char *path; int x, y, w, h, line; } MenuGlyphDef;

static const MenuGlyphDef MENU_TITLE_GLYPHS[MENU_TITLE_GLYPH_COUNT] = {
    { "Assets/Sprites/UI/Menu/glyphs/g00.png", 5, 15, 54, 74, 0 },
    { "Assets/Sprites/UI/Menu/glyphs/g01.png", 70, 15, 36, 73, 0 },
    { "Assets/Sprites/UI/Menu/glyphs/g02.png", 117, 15, 53, 73, 0 },
    { "Assets/Sprites/UI/Menu/glyphs/g03.png", 180, 15, 50, 72, 0 },
    { "Assets/Sprites/UI/Menu/glyphs/g04.png", 241, 15, 52, 72, 0 },
    { "Assets/Sprites/UI/Menu/glyphs/g05.png", 304, 15, 52, 73, 0 },
    { "Assets/Sprites/UI/Menu/glyphs/g06.png", 367, 15, 50, 72, 0 },
    { "Assets/Sprites/UI/Menu/glyphs/g07.png", 428, 4, 14, 31, 0 },
    { "Assets/Sprites/UI/Menu/glyphs/g08.png", 454, 15, 52, 72, 0 },
    { "Assets/Sprites/UI/Menu/glyphs/g09.png", 7, 100, 54, 75, 1 },
    { "Assets/Sprites/UI/Menu/glyphs/g10.png", 71, 101, 54, 74, 1 },
    { "Assets/Sprites/UI/Menu/glyphs/g11.png", 135, 101, 54, 74, 1 },
    { "Assets/Sprites/UI/Menu/glyphs/g12.png", 199, 103, 54, 72, 1 },
    { "Assets/Sprites/UI/Menu/glyphs/g13.png", 264, 102, 52, 74, 1 },
    { "Assets/Sprites/UI/Menu/glyphs/g14.png", 327, 102, 54, 74, 1 },
    { "Assets/Sprites/UI/Menu/glyphs/g15.png", 391, 103, 52, 72, 1 },
    { "Assets/Sprites/UI/Menu/glyphs/g16.png", 454, 103, 52, 72, 1 },
};

#endif
