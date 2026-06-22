// bg_metric.c — mede o alinhamento da textura corpo.png com a AREA CAMINHAVEL
// (mascara de colisao). Para varias escalas/offsets, mapeia cada ponto caminhavel
// para o pixel da imagem e verifica se cai sobre o CORPO (nao no fundo preto).
// Reporta a cobertura (% de pontos caminhaveis sobre o corpo) p/ escolher a melhor
// escala SEM tocar na colisao. Linka map_body.c (stubs de desenho) + raylib (Image).
#include "raylib.h"
#include "../Assets/Maps/map_body.h"
#include <stdio.h>
#include <math.h>

// stubs de desenho exigidos por map_body.c (nao desenhamos nada)
void DrawLineEx(Vector2 a, Vector2 b, float t, Color c){(void)a;(void)b;(void)t;(void)c;}
void DrawCircleV(Vector2 c,float r,Color col){(void)c;(void)r;(void)col;}
void DrawCircleLines(int x,int y,float r,Color c){(void)x;(void)y;(void)r;(void)c;}
void DrawEllipse(int x,int y,float rx,float ry,Color c){(void)x;(void)y;(void)rx;(void)ry;(void)c;}
void DrawEllipseLines(int x,int y,float rx,float ry,Color c){(void)x;(void)y;(void)rx;(void)ry;(void)c;}
void DrawTriangle(Vector2 a,Vector2 b,Vector2 cc,Color c){(void)a;(void)b;(void)cc;(void)c;}
void DrawRectangleRounded(Rectangle r,float a,int s,Color c){(void)r;(void)a;(void)s;(void)c;}
void DrawRectangleRoundedLines(Rectangle r,float a,int s,Color c){(void)r;(void)a;(void)s;(void)c;}
void DrawTextEx(Font f,const char*t,Vector2 p,float s,float sp,Color c){(void)f;(void)t;(void)p;(void)s;(void)sp;(void)c;}
Vector2 MeasureTextEx(Font f,const char*t,float s,float sp){(void)f;(void)t;(void)s;(void)sp;return (Vector2){100,40};}
Color Fade(Color c,float a){(void)a;return c;}
int GetRandomValue(int mn,int mx){return mn;(void)mx;}
bool SpriteAvailable(int id){(void)id;return false;}
void DrawSpriteCentered(int id,Vector2 c,Vector2 s,float r,Color col){(void)id;(void)c;(void)s;(void)r;(void)col;}
Texture2D GetSprite(int id){(void)id;return (Texture2D){0};}

static Image img; static int useAlpha;

static int onBody(float px, float py)
{
    int x = (int)px, y = (int)py;
    if (x < 0 || y < 0 || x >= img.width || y >= img.height) return 0;
    Color c = GetImageColor(img, x, y);
    if (useAlpha) return c.a > 24;
    return (c.r + c.g + c.b) > 45; // corpo (vermelho escuro) vs fundo preto
}

// cobertura: fracao dos pontos caminhaveis que caem sobre o corpo da imagem.
static float coverage(float scale, float dx, float dy, float aspect, int *outTotal, int *outGap)
{
    float h = (float)MAP_HEIGHT * scale, w = h * aspect;
    float tlx = MAP_WIDTH * 0.5f + dx - w * 0.5f;
    float tly = MAP_HEIGHT * 0.5f + dy - h * 0.5f;
    int total = 0, covered = 0;
    for (int wy = 100; wy < MAP_HEIGHT; wy += 30)
        for (int wx = 100; wx < MAP_WIDTH; wx += 30)
        {
            Vector2 p = { (float)wx, (float)wy };
            if (!MapBody_ContainsWithMargin(p, BODY_PLAYER_RADIUS)) continue;
            total++;
            float u = (p.x - tlx) / w, v = (p.y - tly) / h;
            if (onBody(u * img.width, v * img.height)) covered++;
        }
    if (outTotal) *outTotal = total;
    if (outGap) *outGap = total - covered;
    return total ? (float)covered / total : 0.0f;
}

int main(void)
{
    SetTraceLogLevel(LOG_WARNING);
    img = LoadImage("Assets/Sprites/Map/corpo.png");
    if (img.data == NULL) { printf("ERRO: corpo.png nao carregou\n"); return 1; }
    float aspect = (float)img.width / (float)img.height;
    Color corner = GetImageColor(img, 2, 2);
    useAlpha = (corner.a < 250); // fundo transparente?
    printf("img %dx%d aspect=%.4f cornerRGBA=(%d,%d,%d,%d) useAlpha=%d\n",
           img.width, img.height, aspect, corner.r, corner.g, corner.b, corner.a, useAlpha);

    printf("scale  dy     cobertura  lacunas\n");
    for (float s = 0.95f; s <= 1.45f; s += 0.05f)
    {
        int total, gap;
        float cov = coverage(s, 0.0f, 0.0f, aspect, &total, &gap);
        printf("%.2f   0      %6.2f%%    %d/%d\n", s, cov * 100.0f, gap, total);
    }
    // refina com pequenos offsets verticais na melhor faixa
    printf("--- varredura de dy em scale=1.20 ---\n");
    for (float dy = -200; dy <= 200; dy += 50)
    {
        int total, gap; float cov = coverage(1.20f, 0.0f, dy, aspect, &total, &gap);
        printf("scale=1.20 dy=%+5.0f  %6.2f%%  lacunas=%d\n", dy, cov * 100.0f, gap);
    }
    UnloadImage(img);
    return 0;
}
