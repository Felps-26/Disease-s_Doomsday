// bake_collision_mask.c
// Gera Assets/Maps/map_body_mask.h: a occupancy grid (bitmask) da silhueta de
// Assets/Sprites/Map/corpo.png — FONTE AUTORITATIVA da colisão do corpo.
//
// Usa EXATAMENTE a mesma transformação imagem->mundo do renderer (DrawMapBody),
// definida em Assets/Maps/map_body.h (MAPBODY_IMG_SCALE/DX/DY), de modo que a
// área caminhável coincida com a silhueta DESENHADA. Os órgãos NÃO bloqueiam:
// o interior é preenchido por flood-fill do void externo (qualquer "buraco"
// cercado pelo corpo vira corpo). Threshold por luminância (corpo.png é RGB,
// sem canal alpha) ou por alpha quando disponível. Reproduzível: `make collision-mask`.
//
// Build/uso (a partir da pasta do projeto):
//   cc -O2 -Iinclude -I<raylib> tools/bake_collision_mask.c -o bake_collision_mask \
//      <flags raylib> -lm && ./bake_collision_mask
#include "raylib.h"
#include "../include/game.h"          // MAP_WIDTH, MAP_HEIGHT
#include "../Assets/Maps/map_body.h"  // MAPBODY_IMG_*, MAPBODY_BAKE_*, BODY_PLAYER_RADIUS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#define GN    MAPBODY_BAKE_GN
#define NCELL (GN * GN)

static unsigned char fg[NCELL], fg0[NCELL], body[NCELL], ext[NCELL], pass_[NCELL], vis[NCELL];
static unsigned char tmpA[NCELL], tmpB[NCELL];
static int  comp[NCELL], stk[NCELL];
static float sdf[NCELL];

// Dilatação / erosão 8-vizinhança (1 célula). Fora da grade conta como 0.
static void dilate1(const unsigned char *src, unsigned char *dst)
{
    for (int y = 0; y < GN; y++)
        for (int x = 0; x < GN; x++)
        {
            int on = 0;
            for (int dy = -1; dy <= 1 && !on; dy++)
                for (int dx = -1; dx <= 1; dx++)
                {
                    int nx = x + dx, ny = y + dy;
                    if (nx >= 0 && nx < GN && ny >= 0 && ny < GN && src[ny * GN + nx]) { on = 1; break; }
                }
            dst[y * GN + x] = (unsigned char)on;
        }
}
static void erode1(const unsigned char *src, unsigned char *dst)
{
    for (int y = 0; y < GN; y++)
        for (int x = 0; x < GN; x++)
        {
            int all = 1;
            for (int dy = -1; dy <= 1 && all; dy++)
                for (int dx = -1; dx <= 1; dx++)
                {
                    int nx = x + dx, ny = y + dy;
                    if (nx < 0 || nx >= GN || ny < 0 || ny >= GN || !src[ny * GN + nx]) { all = 0; break; }
                }
            dst[y * GN + x] = (unsigned char)all;
        }
}
// Fechamento morfológico (dilata K, erode K) IN-PLACE em m[]: liga membros ao
// tronco nas juntas e fecha frestas finas, preservando o tamanho geral.
static void closeMask(unsigned char *m, int k)
{
    for (int i = 0; i < k; i++) { dilate1(m, tmpA); memcpy(m, tmpA, NCELL); }
    for (int i = 0; i < k; i++) { erode1(m, tmpB); memcpy(m, tmpB, NCELL); }
}

// Transformada de distância 1D ao quadrado (Felzenszwalb & Huttenlocher, O(n)) —
// MESMA do runtime (map_body.c), para que a validação aqui case com o jogo.
static void edt1d(const float *f, float *d, int n, int *v, float *z)
{
    int k = 0; v[0] = 0; z[0] = -FLT_MAX; z[1] = FLT_MAX;
    for (int q = 1; q < n; q++)
    {
        float s = ((f[q] + (float)q * q) - (f[v[k]] + (float)v[k] * v[k])) / (2.0f * q - 2.0f * v[k]);
        while (s <= z[k]) { k--; s = ((f[q] + (float)q * q) - (f[v[k]] + (float)v[k] * v[k])) / (2.0f * q - 2.0f * v[k]); }
        k++; v[k] = q; z[k] = s; z[k + 1] = FLT_MAX;
    }
    k = 0;
    for (int q = 0; q < n; q++)
    {
        while (z[k + 1] < (float)q) k++;
        float dq = (float)(q - v[k]);
        d[q] = dq * dq + f[v[k]];
    }
}
static void computeSq(int seedVal, float *out)
{
    static float f[GN], d[GN]; static int v[GN + 1]; static float z[GN + 2];
    for (int x = 0; x < GN; x++)
    {
        for (int y = 0; y < GN; y++) f[y] = (body[y * GN + x] == seedVal) ? 0.0f : 1e20f;
        edt1d(f, d, GN, v, z);
        for (int y = 0; y < GN; y++) out[y * GN + x] = d[y];
    }
    for (int y = 0; y < GN; y++)
    {
        for (int x = 0; x < GN; x++) f[x] = out[y * GN + x];
        edt1d(f, d, GN, v, z);
        for (int x = 0; x < GN; x++) out[y * GN + x] = d[x];
    }
}

int main(int argc, char **argv)
{
    const char *imgPath = (argc > 1) ? argv[1] : "Assets/Sprites/Map/corpo.png";
    const char *outPath = (argc > 2) ? argv[2] : "Assets/Maps/map_body_mask.h";
    SetTraceLogLevel(LOG_WARNING);

    Image img = LoadImage(imgPath);
    if (img.data == NULL) { fprintf(stderr, "ERRO: nao carregou %s\n", imgPath); return 1; }
    int iw = img.width, ih = img.height;
    Color corner = GetImageColor(img, 1, 1);
    int useAlpha = (corner.a < 250); // corpo.png atual é RGB (alpha=255) -> usa luminância

    // Transformação imagem -> mundo (IDÊNTICA ao renderer DrawMapBody).
    float aspect = (float)iw / (float)ih;
    float wh = (float)MAP_HEIGHT * MAPBODY_IMG_SCALE;
    float ww = wh * aspect;
    float cx = MAP_WIDTH * 0.5f + MAPBODY_IMG_DX, cy = MAP_HEIGHT * 0.5f + MAPBODY_IMG_DY;
    float tlx = cx - ww * 0.5f, tly = cy - wh * 0.5f;
    float cell = (float)MAP_WIDTH / (float)GN;

    // 1) Foreground bruto por célula (amostra o pixel correspondente no mundo).
    for (int gy = 0; gy < GN; gy++)
        for (int gx = 0; gx < GN; gx++)
        {
            float wx = (gx + 0.5f) * cell, wy = (gy + 0.5f) * cell;
            float u = (wx - tlx) / ww, v = (wy - tly) / wh;
            int on = 0;
            if (u >= 0.0f && u < 1.0f && v >= 0.0f && v < 1.0f)
            {
                int px = (int)(u * iw); if (px >= iw) px = iw - 1; if (px < 0) px = 0;
                int py = (int)(v * ih); if (py >= ih) py = ih - 1; if (py < 0) py = 0;
                Color c = GetImageColor(img, px, py);
                if (useAlpha) on = (c.a > 40);
                else { float lum = 0.299f * c.r + 0.587f * c.g + 0.114f * c.b; on = (lum > (float)MAPBODY_BAKE_LUMA); }
            }
            fg[gy * GN + gx] = (unsigned char)on;
        }

    // Guarda a silhueta BRUTA (só threshold) para o diagnóstico "jogável sobre
    // o vazio visual" (amarelo): células marcadas como corpo onde a ARTE não tem
    // pixel desenhado — exatamente o problema que queremos eliminar.
    memcpy(fg0, fg, NCELL);

    // 1b) Fechamento morfológico: liga os membros ao tronco nas juntas
    //     (ombros/quadris) e fecha frestas finas — evita "bolsões" desconectados.
    if (MAPBODY_BAKE_CLOSE > 0) closeMask(fg, MAPBODY_BAKE_CLOSE);

    // 2) Flood-fill do VOID EXTERNO a partir da borda, andando só por !fg (4-conn).
    //    body = !ext  => preenche automaticamente buracos internos (órgãos escuros
    //    cercados pelo corpo viram corpo, então NÃO bloqueiam o movimento).
    memset(ext, 0, sizeof ext);
    int sp = 0;
    for (int x = 0; x < GN; x++)
    {
        int a = x, b = (GN - 1) * GN + x;
        if (!fg[a] && !ext[a]) { ext[a] = 1; stk[sp++] = a; }
        if (!fg[b] && !ext[b]) { ext[b] = 1; stk[sp++] = b; }
    }
    for (int y = 0; y < GN; y++)
    {
        int a = y * GN, b = y * GN + GN - 1;
        if (!fg[a] && !ext[a]) { ext[a] = 1; stk[sp++] = a; }
        if (!fg[b] && !ext[b]) { ext[b] = 1; stk[sp++] = b; }
    }
    while (sp > 0)
    {
        int i = stk[--sp], x = i % GN, y = i / GN;
        int nb[4] = { x > 0 ? i - 1 : -1, x < GN - 1 ? i + 1 : -1, y > 0 ? i - GN : -1, y < GN - 1 ? i + GN : -1 };
        for (int k = 0; k < 4; k++) { int n = nb[k]; if (n >= 0 && !fg[n] && !ext[n]) { ext[n] = 1; stk[sp++] = n; } }
    }
    for (int i = 0; i < NCELL; i++) body[i] = ext[i] ? 0 : 1;

    // 3) Mantém apenas o MAIOR componente conexo do corpo (remove specks isolados).
    for (int i = 0; i < NCELL; i++) comp[i] = -1;
    int cid = 0, best = -1, bestSize = 0;
    for (int s = 0; s < NCELL; s++)
    {
        if (!body[s] || comp[s] >= 0) continue;
        int head = 0, tail = 0, sz = 0; stk[tail++] = s; comp[s] = cid;
        while (head < tail)
        {
            int i = stk[head++], x = i % GN, y = i / GN; sz++;
            int nb[4] = { x > 0 ? i - 1 : -1, x < GN - 1 ? i + 1 : -1, y > 0 ? i - GN : -1, y < GN - 1 ? i + GN : -1 };
            for (int k = 0; k < 4; k++) { int n = nb[k]; if (n >= 0 && body[n] && comp[n] < 0) { comp[n] = cid; stk[tail++] = n; } }
        }
        if (sz > bestSize) { bestSize = sz; best = cid; }
        cid++;
    }
    for (int i = 0; i < NCELL; i++) body[i] = (comp[i] == best) ? 1 : 0;

    // 3b) MÉTRICA DE FIDELIDADE: "vazio externo jogável". Inunda o EXTERIOR da
    //     ARTE BRUTA (fg0, sem fechamento) a partir da borda por !fg0. Qualquer
    //     célula de corpo que caia nesse exterior é área caminhável SOBRE o vazio
    //     visual (ex.: axila/entre-membros que o fechamento ligou indevidamente).
    //     Buracos internos de órgão NÃO entram aqui (são cercados pela arte).
    static unsigned char ext0[NCELL];
    memset(ext0, 0, sizeof ext0);
    sp = 0;
    for (int x = 0; x < GN; x++) {
        int a = x, b = (GN - 1) * GN + x;
        if (!fg0[a] && !ext0[a]) { ext0[a] = 1; stk[sp++] = a; }
        if (!fg0[b] && !ext0[b]) { ext0[b] = 1; stk[sp++] = b; }
    }
    for (int y = 0; y < GN; y++) {
        int a = y * GN, b = y * GN + GN - 1;
        if (!fg0[a] && !ext0[a]) { ext0[a] = 1; stk[sp++] = a; }
        if (!fg0[b] && !ext0[b]) { ext0[b] = 1; stk[sp++] = b; }
    }
    while (sp > 0) {
        int i = stk[--sp], x = i % GN, y = i / GN;
        int nb[4] = { x > 0 ? i - 1 : -1, x < GN - 1 ? i + 1 : -1, y > 0 ? i - GN : -1, y < GN - 1 ? i + GN : -1 };
        for (int k = 0; k < 4; k++) { int n = nb[k]; if (n >= 0 && !fg0[n] && !ext0[n]) { ext0[n] = 1; stk[sp++] = n; } }
    }
    int voidWalk = 0;
    for (int i = 0; i < NCELL; i++) if (body[i] && ext0[i]) voidWalk++;

    // 4) SDF (px de mundo), bbox, centro seguro e conectividade do PASSÁVEL
    //    (disco do jogador), exatamente como o runtime/teste validam.
    computeSq(0, sdf); // dist² ao void externo
    float bestd = -1.0f; int sbx = GN / 2, sby = GN / 2;
    int minx = GN, miny = GN, maxx = 0, maxy = 0, bodyCells = 0;
    for (int y = 0; y < GN; y++)
        for (int x = 0; x < GN; x++)
        {
            int i = y * GN + x;
            float d = sqrtf(sdf[i]) * cell; sdf[i] = d;
            if (body[i]) { bodyCells++;
                if (x < minx) minx = x; if (x > maxx) maxx = x; if (y < miny) miny = y; if (y > maxy) maxy = y;
                if (d > bestd) { bestd = d; sbx = x; sby = y; } }
        }
    for (int i = 0; i < NCELL; i++) pass_[i] = (sdf[i] >= BODY_PLAYER_RADIUS) ? 1 : 0;
    memset(vis, 0, sizeof vis);
    int passable = 0, reached = 0;
    for (int i = 0; i < NCELL; i++) if (pass_[i]) passable++;
    int start = sby * GN + sbx;
    if (pass_[start])
    {
        int head = 0, tail = 0; stk[tail++] = start; vis[start] = 1;
        while (head < tail)
        {
            int i = stk[head++], x = i % GN, y = i / GN; reached++;
            int nb[4] = { x > 0 ? i - 1 : -1, x < GN - 1 ? i + 1 : -1, y > 0 ? i - GN : -1, y < GN - 1 ? i + GN : -1 };
            for (int k = 0; k < 4; k++) { int n = nb[k]; if (n >= 0 && pass_[n] && !vis[n]) { vis[n] = 1; stk[tail++] = n; } }
        }
    }
    int disconnected = passable - reached;

    // Diagnóstico visual (gitignored): void=preto, corpo(parede fina)=vinho,
    // passável alcançável=verde, passável DESCONECTADO=vermelho vivo, centro=ciano.
    {
        Image dbg = GenImageColor(GN, GN, BLACK);
        for (int y = 0; y < GN; y++)
            for (int x = 0; x < GN; x++)
            {
                int i = y * GN + x;
                Color c = (Color){ 0, 0, 0, 255 };
                if (body[i]) c = (Color){ 90, 30, 40, 255 };
                if (pass_[i]) c = vis[i] ? (Color){ 60, 200, 90, 255 } : (Color){ 255, 40, 40, 255 };
                // AMARELO: corpo onde a arte não desenhou, mas CERCADO pela arte
                // (buraco interno de órgão preenchido — OK, órgãos não bloqueiam).
                // LARANJA: corpo sobre o EXTERIOR da arte = vazio visual jogável
                // (axila/entre-membros indevido) — é o que queremos zerar.
                if (body[i] && !fg0[i]) c = (Color){ 235, 215, 0, 255 };
                if (body[i] && ext0[i]) c = (Color){ 255, 120, 0, 255 };
                ImageDrawPixel(&dbg, x, y, c);
            }
        ImageDrawPixel(&dbg, sbx, sby, (Color){ 0, 229, 255, 255 });
        ExportImage(dbg, "tools/mask_debug.png");
        UnloadImage(dbg);
    }

    printf("img %dx%d aspect=%.4f useAlpha=%d thr=%d\n", iw, ih, aspect, useAlpha, MAPBODY_BAKE_LUMA);
    printf("GN=%d cell=%.3f bodyCells=%d bbox=cell(%d,%d)-(%d,%d) safe=world(%.0f,%.0f) maxClear=%.0fpx\n",
           GN, cell, bodyCells, minx, miny, maxx, maxy, (sbx + 0.5f) * cell, (sby + 0.5f) * cell, bestd);
    printf("passaveis(>=%.0fpx)=%d alcancados=%d desconectados=%d\n",
           (float)BODY_PLAYER_RADIUS, passable, reached, disconnected);
    printf("vazio-externo-jogavel(close=%d): %d celulas (%.2f%% do corpo) [LARANJA no debug]\n",
           MAPBODY_BAKE_CLOSE, voidWalk, 100.0f * (float)voidWalk / (float)(bodyCells > 0 ? bodyCells : 1));
    if (disconnected != 0) { fprintf(stderr, "ERRO: area caminhavel desconectada (%d celulas); ajuste threshold/transform.\n", disconnected); UnloadImage(img); return 2; }
    if (bodyCells < NCELL / 40) { fprintf(stderr, "ERRO: corpo pequeno demais (%d celulas); threshold/transform?\n", bodyCells); UnloadImage(img); return 3; }
    // Guarda de fidelidade (hitbox): rejeita bakes com muita área jogável SOBRE o
    // vazio visual — sintoma de fechamento morfológico exagerado religando axila/
    // entre-membros ao tronco. Margem folgada acima do residual de anti-aliasing
    // (~0.33% em CLOSE=1); CLOSE=4 (~1.5%) reprova de propósito.
    {
        float voidPct = 100.0f * (float)voidWalk / (float)(bodyCells > 0 ? bodyCells : 1);
        if (voidPct > 1.2f) { fprintf(stderr, "ERRO: vazio externo jogavel alto (%.2f%% > 1.2%%); reduza MAPBODY_BAKE_CLOSE ou ajuste a arte.\n", voidPct); UnloadImage(img); return 5; }
    }

    // 5) Empacota row-major, LSB-first, e escreve o header com PROVENIÊNCIA.
    int bytes = (NCELL + 7) / 8;
    unsigned char *packed = (unsigned char *)calloc(bytes, 1);
    for (int i = 0; i < NCELL; i++) if (body[i]) packed[i >> 3] |= (unsigned char)(1u << (i & 7));

    FILE *f = fopen(outPath, "w");
    if (!f) { fprintf(stderr, "ERRO: nao escreveu %s\n", outPath); free(packed); UnloadImage(img); return 4; }
    fprintf(f, "// map_body_mask.h - GERADO pelo baker de colisao (make collision-mask). NAO editar a mao.\n");
    fprintf(f, "// Origem    : %s (%dx%d, %s)\n", imgPath, iw, ih, useAlpha ? "RGBA/alpha" : "RGB/luminancia");
    fprintf(f, "// Mundo     : %dx%d   Grade: %dx%d (celula %.3f px de mundo)\n", MAP_WIDTH, MAP_HEIGHT, GN, GN, cell);
    fprintf(f, "// Transform : aspecto preservado, centrado; altura=MAP_HEIGHT*%.2f, dx=%.1f, dy=%.1f (ver map_body.h)\n",
            (double)MAPBODY_IMG_SCALE, (double)MAPBODY_IMG_DX, (double)MAPBODY_IMG_DY);
    fprintf(f, "// Criterio  : %s%d ; buracos internos preenchidos ; maior componente conexo\n",
            useAlpha ? "alpha>" : "luminancia>", useAlpha ? 40 : MAPBODY_BAKE_LUMA);
    fprintf(f, "// Comando   : make collision-mask  (ou python3 tools/bake_collision_mask.py)\n");
    fprintf(f, "// Bit (gy*GN+gx), LSB-first. 1=dentro do corpo (jogavel), 0=void externo.\n");
    fprintf(f, "#ifndef MAP_BODY_MASK_H\n#define MAP_BODY_MASK_H\n\n");
    fprintf(f, "#define MAPBODY_MASK_GN %d\n", GN);
    fprintf(f, "#define MAPBODY_MASK_BYTES %d\n\n", bytes);
    fprintf(f, "static const unsigned char MAPBODY_MASK_BITS[MAPBODY_MASK_BYTES] = {\n");
    for (int i = 0; i < bytes; i += 20)
    {
        fprintf(f, "  ");
        for (int j = i; j < i + 20 && j < bytes; j++) fprintf(f, "%d,", packed[j]);
        fprintf(f, "\n");
    }
    fprintf(f, "};\n\n#endif // MAP_BODY_MASK_H\n");
    fclose(f);
    printf("escrito: %s  bytes: %d\n", outPath, bytes);

    free(packed);
    UnloadImage(img);
    return 0;
}
