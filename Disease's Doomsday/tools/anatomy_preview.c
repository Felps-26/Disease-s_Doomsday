// anatomy_preview.c — renderiza DrawMapBody() (camada anatômica procedural) em
// PNGs para revisão visual offline: corpo completo + recortes (cabeça, tórax,
// abdome, pelve). Linka apenas Assets/Maps/map_body.c + raylib, com stubs do
// sprite_manager para forçar o desenho PROCEDURAL (sem PNGs).
//
// Build (a partir da pasta do projeto):
//   cc -O2 -Iinclude -I/opt/homebrew/include tools/anatomy_preview.c \
//      Assets/Maps/map_body.c -o anatomy_preview -L/opt/homebrew/lib -lraylib \
//      -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio \
//      -framework CoreVideo -lm && ./anatomy_preview
#include "raylib.h"
#include "../Assets/Maps/map_body.h"
#include <stdio.h>

// Stubs do sprite_manager: força o caminho procedural de DrawMapBody.
bool SpriteAvailable(int id) { (void)id; return false; }
void DrawSpriteCentered(int id, Vector2 c, Vector2 s, float r, Color col) { (void)id;(void)c;(void)s;(void)r;(void)col; }

static Font gFont;

// Renderiza uma "câmera" do mundo (centro worldC, span worldSpan px) num PNG NxN.
static void Shot(const char *name, Vector2 worldC, float worldSpan, int px, int world, int wave)
{
    RenderTexture2D rt = LoadRenderTexture(px, px);
    Camera2D cam = { 0 };
    cam.target = worldC;
    cam.offset = (Vector2){ px / 2.0f, px / 2.0f };
    cam.zoom   = (float)px / worldSpan;

    BeginTextureMode(rt);
    ClearBackground((Color){ 7, 4, 8, 255 });
    BeginMode2D(cam);
    DrawMapBody(gFont, world, wave, 0.8f); // time fixo p/ snapshot estável
    EndMode2D();
    EndTextureMode();

    Image img = LoadImageFromTexture(rt.texture);
    ImageFlipVertical(&img);
    char path[160]; snprintf(path, sizeof(path), "tools/anat_%s.png", name);
    ExportImage(img, path);
    UnloadImage(img);
    UnloadRenderTexture(rt);
    printf("[anatomy_preview] %s\n", path);
}

int main(void)
{
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(64, 64, "anatomy preview");
    gFont = GetFontDefault();

    // Corpo inteiro (mundo 4000x4000). Mundo 1 (bactérias) foca pulmões na onda 1.
    Shot("full_w1",  (Vector2){ 2000, 2000 }, 4000.0f, 1200, 0, 1);
    Shot("full_w2",  (Vector2){ 2000, 2000 }, 4000.0f, 1200, 0, 2); // foco corrente sanguínea
    Shot("full_hosp",(Vector2){ 2000, 2000 }, 4000.0f, 1200, 0, 6); // foco hospitalar (intestino)

    // Recortes regionais (span menor = mais zoom).
    Shot("head",   (Vector2){ 2000, 520 },  1000.0f, 900, 0, 1);
    Shot("thorax", (Vector2){ 2000, 1450 }, 1300.0f, 900, 0, 1);
    Shot("abdomen",(Vector2){ 2000, 1850 }, 1300.0f, 900, 0, 1);
    Shot("pelvis", (Vector2){ 2000, 2300 }, 1300.0f, 900, 0, 6);

    CloseWindow();
    return 0;
}
