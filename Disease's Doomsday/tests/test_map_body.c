// test_map_body.c
// ACEITAÇÃO (Fase 1): valida a nova representação de colisão do corpo (occupancy
// grid + signed distance field baked de corpo.png). Sem janela gráfica: stubs
// das funções de desenho da raylib. Cobre margem/borda, projeção (ApplyCollision
// sem teleporte/oscilação), conectividade fina e enquadramento do minimapa.
//
// Build & run (a partir da pasta do projeto):
//   gcc -Iinclude -I<raylib/headers> tests/test_map_body.c \
//       Assets/Maps/map_body.c -o /tmp/test_mb -lm && /tmp/test_mb
#include "raylib.h"
#include "../Assets/Maps/map_body.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// ---- Stubs das funções da raylib referenciadas por map_body.c (desenho) ----
void DrawLineEx(Vector2 a, Vector2 b, float t, Color c) { (void)a;(void)b;(void)t;(void)c; }
void DrawCircleV(Vector2 c, float r, Color col) { (void)c;(void)r;(void)col; }
void DrawCircleLines(int x, int y, float r, Color c) { (void)x;(void)y;(void)r;(void)c; }
void DrawEllipse(int x, int y, float rx, float ry, Color c) { (void)x;(void)y;(void)rx;(void)ry;(void)c; }
void DrawEllipseLines(int x, int y, float rx, float ry, Color c) { (void)x;(void)y;(void)rx;(void)ry;(void)c; }
void DrawTriangle(Vector2 a, Vector2 b, Vector2 cc, Color c) { (void)a;(void)b;(void)cc;(void)c; }
void DrawRectangleRounded(Rectangle r, float a, int s, Color c) { (void)r;(void)a;(void)s;(void)c; }
void DrawRectangleRoundedLines(Rectangle r, float a, int s, Color c) { (void)r;(void)a;(void)s;(void)c; }
void DrawTextEx(Font f, const char *t, Vector2 p, float s, float sp, Color c) { (void)f;(void)t;(void)p;(void)s;(void)sp;(void)c; }
Vector2 MeasureTextEx(Font f, const char *t, float s, float sp) { (void)f;(void)t;(void)s;(void)sp; return (Vector2){100,40}; }
Color Fade(Color c, float a) { (void)a; return c; }
int GetRandomValue(int mn, int mx) { return mn + rand() % (mx - mn + 1); }
bool SpriteAvailable(int id) { (void)id; return false; }
void DrawSpriteCentered(int id, Vector2 c, Vector2 s, float r, Color col) { (void)id;(void)c;(void)s;(void)r;(void)col; }

static float frand(float a, float b){ return a + (b-a)*((float)rand()/(float)RAND_MAX); }
static float dist(Vector2 a, Vector2 b){ return sqrtf((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)); }

int main(void)
{
    srand(2026);
    int fail = 0;

    // ------------------------------------------------------------------
    // 1) SOLIDEZ DA MARGEM: se ContainsWithMargin(p, m) é verdadeiro, então o
    //    disco de raio m ao redor de p deve estar inteiramente dentro do corpo.
    // ------------------------------------------------------------------
    int marginChecked = 0, marginViol = 0;
    const float margins[3] = { BODY_PLAYER_RADIUS, BODY_ENEMY_RADIUS, CORE_SPAWN_MARGIN };
    for (int t = 0; t < 60000; t++)
    {
        Vector2 p = { frand(0, MAP_WIDTH), frand(0, MAP_HEIGHT) };
        float m = margins[t % 3];
        if (!MapBody_ContainsWithMargin(p, m)) continue;
        marginChecked++;
        // amostra o perímetro do disco; tolerância de ~1.6 células (20px) para a
        // discretização do grid de 12.5px (a margem é sólida a menos de 2 células).
        for (int a = 0; a < 16; a++)
        {
            float ang = (float)a * (2.0f * PI / 16.0f);
            Vector2 q = { p.x + cosf(ang) * (m - 20.0f), p.y + sinf(ang) * (m - 20.0f) };
            if (!MapBody_Contains(q)) { marginViol++; break; }
        }
    }
    printf("1) Margem: validados=%d  violacoes=%d\n", marginChecked, marginViol);
    if (marginViol != 0) fail++;

    // ------------------------------------------------------------------
    // 2) PROJEÇÃO (ApplyCollision): pontos já com folga não se movem; pontos
    //    perto da parede / ligeiramente fora são projetados para DENTRO com
    //    folga, sem teleporte (movimento limitado) e sem oscilação (idempotente).
    // ------------------------------------------------------------------
    int projInside = 0, projBadClear = 0, projTeleport = 0, projOscillate = 0, projStable = 0, projStableMoved = 0;
    float projMaxMove = 0.0f;
    const float R = BODY_PLAYER_RADIUS;
    for (int t = 0; t < 40000; t++)
    {
        Vector2 p = { frand(0, MAP_WIDTH), frand(0, MAP_HEIGHT) };
        // restringe a violações REALISTAS de 1 frame: dentro, ou no máximo ~45px
        // fora da borda (entidades nunca pulam 150px/frame; spawns distantes usam
        // FindSpawnPoint, não ApplyCollision).
        if (!MapBody_Contains(p))
        {
            bool near = false;
            for (int a = 0; a < 12 && !near; a++)
            { float an=a*(PI/6.0f); Vector2 q={p.x+cosf(an)*45.0f,p.y+sinf(an)*45.0f}; if (MapBody_Contains(q)) near=true; }
            if (!near) continue;
        }
        bool hadClear = MapBody_ContainsWithMargin(p, R);
        Vector2 orig = p;
        MapBody_ApplyCollision(&p, R);

        // a) deve terminar dentro com folga (tolerância de 1 célula)
        if (!MapBody_ContainsWithMargin(p, R - 12.5f)) projBadClear++;
        else projInside++;
        // b) projeção LOCAL (sem teleporte p/ região distante). Admitimos pontos
        //    a <=45px da borda; alcançar folga R exige <=~45+R + folga diagonal.
        //    Um teleporte real (p/ o tórax) seria de centenas de px.
        float mv = dist(p, orig);
        if (mv > projMaxMove) projMaxMove = mv;
        if (mv > 150.0f) projTeleport++;
        // c) idempotência: aplicar de novo quase não move
        Vector2 p2 = p; MapBody_ApplyCollision(&p2, R);
        if (dist(p2, p) > 2.0f) projOscillate++;
        // d) pontos que já tinham folga praticamente não se movem
        if (hadClear) { projStable++; if (dist(p, orig) > 1.5f) projStableMoved++; }
    }
    printf("2) Projecao: dentro=%d folgaRuim=%d teleporte=%d oscilou=%d maxMove=%.0fpx | jaFolgados=%d moveram=%d\n",
           projInside, projBadClear, projTeleport, projOscillate, projMaxMove, projStable, projStableMoved);
    if (projBadClear || projTeleport || projOscillate || projStableMoved) fail++;

    // ------------------------------------------------------------------
    // 3) CONECTIVIDADE FINA: amostragem em passo 25px; toda célula transitável
    //    pelo jogador pertence à mesma região do centro seguro.
    // ------------------------------------------------------------------
    {
        enum { STEP = 25, COLS = MAP_WIDTH / STEP, ROWS = MAP_HEIGHT / STEP, CELLS = COLS * ROWS };
        static unsigned char passable[CELLS]; static unsigned char visited[CELLS]; static int queue[CELLS];
        int passableCount = 0, reached = 0, start = -1;
        Vector2 safe = MapBody_GetSafeCenter();
        for (int y = 0; y < ROWS; y++) for (int x = 0; x < COLS; x++)
        {
            int i = y*COLS+x; Vector2 p = { x*STEP+STEP*0.5f, y*STEP+STEP*0.5f };
            passable[i] = MapBody_ContainsWithMargin(p, BODY_PLAYER_RADIUS);
            if (passable[i]) passableCount++;
            if (fabsf(p.x-safe.x)<=STEP*0.5f && fabsf(p.y-safe.y)<=STEP*0.5f) start = i;
        }
        int disc = passableCount;
        if (start >= 0 && passable[start])
        {
            int head=0, tail=0; queue[tail++]=start; visited[start]=1;
            while (head<tail){ int i=queue[head++],x=i%COLS,y=i/COLS; reached++;
                int nx[4]={x-1,x+1,x,x}, ny[4]={y,y,y-1,y+1};
                for(int d=0;d<4;d++){ if(nx[d]<0||nx[d]>=COLS||ny[d]<0||ny[d]>=ROWS)continue;
                    int n=ny[d]*COLS+nx[d]; if(passable[n]&&!visited[n]){visited[n]=1;queue[tail++]=n;} } }
            disc = passableCount - reached;
        }
        printf("3) Conectividade(passo25): transitaveis=%d alcancadas=%d desconectadas=%d\n", passableCount, reached, disc);
        if (disc != 0) fail++;
    }

    // ------------------------------------------------------------------
    // 4) MINIMAPA / ENQUADRAMENTO: a bbox do corpo contém todo ponto do corpo,
    //    está dentro do mundo, e o centro seguro é interior profundo.
    // ------------------------------------------------------------------
    {
        Rectangle b = MapBody_WorldBounds();
        int outOfBounds = 0, normViol = 0;
        for (int t = 0; t < 40000; t++)
        {
            Vector2 p = { frand(0, MAP_WIDTH), frand(0, MAP_HEIGHT) };
            if (!MapBody_Contains(p)) continue;
            if (p.x < b.x-1 || p.y < b.y-1 || p.x > b.x+b.width+1 || p.y > b.y+b.height+1) outOfBounds++;
            float u=(p.x-b.x)/b.width, v=(p.y-b.y)/b.height;
            if (u < -0.001f || u > 1.001f || v < -0.001f || v > 1.001f) normViol++;
        }
        Vector2 safe = MapBody_GetSafeCenter();
        bool safeInside = (safe.x>=b.x && safe.x<=b.x+b.width && safe.y>=b.y && safe.y<=b.y+b.height);
        bool boundsInWorld = (b.x>=0 && b.y>=0 && b.x+b.width<=MAP_WIDTH && b.y+b.height<=MAP_HEIGHT);
        bool safeDeep = MapBody_ContainsWithMargin(safe, 100.0f);
        printf("4) Minimapa: bbox=(%.0f,%.0f %.0fx%.0f) foraBbox=%d normViol=%d safeInside=%d boundsInWorld=%d safeDeep=%d\n",
               b.x,b.y,b.width,b.height, outOfBounds, normViol, safeInside, boundsInWorld, safeDeep);
        if (outOfBounds || normViol || !safeInside || !boundsInWorld || !safeDeep) fail++;
    }

    printf("%s\n", fail==0 ? "RESULTADO: PASSOU (colisao/margem/projecao/conectividade/minimapa)"
                           : "RESULTADO: FALHOU");
    return fail==0 ? 0 : 1;
}
