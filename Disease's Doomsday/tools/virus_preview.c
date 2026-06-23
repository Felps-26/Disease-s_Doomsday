// virus_preview.c — renderiza as cinco identidades virais procedurais (fallback
// sem PNG) num único PNG, com e sem capsídeo e o chefe em escala. Linka
// Assets/@models/enemy_model.c + src/entities/enemy.c + raylib; stubs do
// sprite_manager forçam o caminho procedural.
//
// Build (a partir da pasta do projeto):
//   cc -O2 -Iinclude -I/opt/homebrew/include tools/virus_preview.c \
//      Assets/@models/enemy_model.c src/entities/enemy.c -o virus_preview \
//      -L/opt/homebrew/lib -lraylib -framework OpenGL -framework Cocoa \
//      -framework IOKit -framework CoreAudio -framework CoreVideo -lm && ./virus_preview
#include "raylib.h"
#include "../include/game.h"
#include "../Assets/@models/enemy_model.h"
#include "../include/sprite_manager.h"
#include <stdio.h>
#include <string.h>

// Stubs do sprite_manager: força o desenho procedural.
bool SpriteAvailable(SpriteID id) { (void)id; return false; }
void DrawSpriteCentered(SpriteID id, Vector2 c, Vector2 s, float r, Color col) { (void)id;(void)c;(void)s;(void)r;(void)col; }

static Font gFont;

static Enemy MakeEnemy(int type, int wave, int hpOverridePct)
{
    Enemy e; memset(&e, 0, sizeof(e));
    EnemyInitFromArchetype(&e, type, wave, 1.0f);
    e.active = true; e.state = IDLE; e.spawnAnim = 1.0f; e.animTime = 0.7f;
    if (hpOverridePct >= 0) e.hp = e.maxHp * hpOverridePct / 100;
    return e;
}

// Desenha um vírus rotulado numa célula.
static void Cell(int type, const char *label, float baseSize, Vector2 at, int hpPct)
{
    Enemy e = MakeEnemy(type, 4, hpPct);
    const EnemyArchetype *a = EnemyArchetypeFor(type);
    float size = baseSize * (a ? a->sizeScale : 1.0f);
    DrawEnemyModel(&e, at, size, 0.0f, 1.0f, 1.0f);
    // anel de capsídeo (como no jogo) se houver escudo ativo
    if (e.shieldActive && e.shieldHp > 0)
    {
        float shR = size * 1.6f;
        DrawCircleLines((int)at.x, (int)at.y, shR, Fade((Color){ 120, 200, 255, 255 }, 0.7f));
        DrawCircleLines((int)at.x, (int)at.y, shR * 0.92f, Fade((Color){ 120, 200, 255, 255 }, 0.3f));
    }
    Vector2 ts = MeasureTextEx(gFont, label, 20.0f, 1.0f);
    DrawTextEx(gFont, label, (Vector2){ at.x - ts.x / 2.0f, at.y + size * 1.7f + 8.0f }, 20.0f, 1.0f, RAYWHITE);
}

int main(void)
{
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(64, 64, "virus preview");
    gFont = GetFontDefault();

    const int W = 1200, H = 520;
    RenderTexture2D rt = LoadRenderTexture(W, H);
    BeginTextureMode(rt);
    ClearBackground((Color){ 14, 10, 20, 255 });

    float y1 = 150.0f;
    Cell(ETYPE_VIRUS_SWARM,  "ENXAME (rino)",   55.0f, (Vector2){ 150,  y1 }, -1);
    Cell(ETYPE_VIRUS_MELEE,  "MELEE (dengue)",  55.0f, (Vector2){ 380,  y1 }, -1);
    Cell(ETYPE_VIRUS_RANGED, "ATIRADOR (flu)",  55.0f, (Vector2){ 610,  y1 }, -1);
    Cell(ETYPE_VIRUS_ELITE,  "ELITE (sarampo)", 55.0f, (Vector2){ 880,  y1 }, -1);

    float y2 = 380.0f;
    Cell(ETYPE_VIRUS_BOSS,   "CHEFE intacto",   120.0f, (Vector2){ 320, y2 }, 100);
    Cell(ETYPE_VIRUS_BOSS,   "CHEFE capsideo roto", 120.0f, (Vector2){ 760, y2 }, 20);

    EndTextureMode();

    Image img = LoadImageFromTexture(rt.texture);
    ImageFlipVertical(&img);
    ExportImage(img, "tools/virus_roster.png");
    UnloadImage(img);
    UnloadRenderTexture(rt);
    printf("[virus_preview] tools/virus_roster.png\n");
    CloseWindow();
    return 0;
}
