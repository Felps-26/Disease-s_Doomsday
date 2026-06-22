// doctor_model.c
// Assistente do tutorial em pixel art procedural animado (placeholder de alta
// qualidade). Estados: idle (flutua + pisca), talk (boca), react (sobrancelhas).
//
// SUBSTITUIÇÃO POR ARTE FINAL (sprite sheet): exportar um sheet horizontal de
// 32x32 px por quadro, 5 quadros nesta ordem [idle, blink, talk_open, talk_mid,
// react], paleta de ~16 cores no tema biomédico (jaleco branco, lentes ciano
// #5AD2EB, cabelo claro, acento verde #00C878). Carregar via pipeline de
// sprites e escolher o frame conforme talking/reactT/blink já calculados aqui.
#include "doctor_model.h"
#include <math.h>

#define DOC_CYAN (Color){ 0, 229, 255, 255 }

typedef struct DoctorPalette {
    Color hair, skin, skinSh, frame, lens, coat, accent, mouth, mirror, mirrorHi;
} DoctorPalette;

// Desenha uma "célula" do grid de pixels (snap inteiro p/ nitidez pixel-art).
static void DocPx(float ox, float oy, float px, int gx, int gy, int gw, int gh, Color c)
{
    DrawRectangle((int)(ox + gx * px), (int)(oy + gy * px),
                  (int)(gw * px + 0.5f), (int)(gh * px + 0.5f), c);
}

void DrawTutorialDoctor(Vector2 center, float radius, float time, bool talking, float reactT)
{
    // Moldura circular (encaixa no espaço do antigo círculo "Ab").
    float bob = sinf(time * 2.2f) * (radius * 0.05f);
    Vector2 c = { center.x, center.y + bob };
    DrawCircleV(c, radius + 3.0f, Fade((Color){ 4, 14, 20, 255 }, 0.95f));
    DrawCircleV(c, radius + 1.0f, (Color){ 8, 26, 34, 255 });

    DoctorPalette p = {
        .hair     = (Color){ 226, 230, 238, 255 },
        .skin     = (Color){ 236, 184, 152, 255 },
        .skinSh   = (Color){ 206, 150, 120, 255 },
        .frame    = (Color){ 46, 52, 64, 255 },
        .lens     = (Color){ 90, 210, 235, 255 },
        .coat     = (Color){ 240, 246, 252, 255 },
        .accent   = (Color){ 0, 200, 120, 255 },
        .mouth    = (Color){ 120, 40, 50, 255 },
        .mirror   = (Color){ 205, 232, 242, 255 },
        .mirrorHi = (Color){ 0, 200, 255, 255 },
    };

    // Grid 18x18 centrado no portrait. O span é levemente MENOR que o diâmetro
    // (1.7r) para que cantos do jaleco/cabelo não ultrapassem a moldura circular.
    const int G = 18;
    float px = (radius * 1.7f) / (float)G;
    float ox = c.x - (G * px) * 0.5f;
    float oy = c.y - (G * px) * 0.5f;

    // --- Cabelo bagunçado (cientista maluco): topo + tufos ---
    for (int gx = 3; gx <= 14; gx++) { int spike = ((gx * 7) % 3); DocPx(ox, oy, px, gx, (spike == 0 ? 0 : 1), 1, 1, p.hair); }
    DocPx(ox, oy, px, 3, 1, 12, 2, p.hair);
    DocPx(ox, oy, px, 2, 2, 2, 3, p.hair);   // costeleta esq
    DocPx(ox, oy, px, 14, 2, 2, 3, p.hair);  // costeleta dir

    // --- Rosto ---
    DocPx(ox, oy, px, 4, 3, 10, 9, p.skin);
    DocPx(ox, oy, px, 4, 11, 10, 1, p.skinSh); // queixo sombreado

    // --- Espelho frontal de médico (testa) ---
    DocPx(ox, oy, px, 8, 2, 2, 2, p.mirror);
    DocPx(ox, oy, px, 8, 2, 1, 1, p.mirrorHi);

    // --- Óculos/goggles: armação + lentes ---
    DocPx(ox, oy, px, 4, 5, 4, 1, p.frame);   // topo esq
    DocPx(ox, oy, px, 10, 5, 4, 1, p.frame);  // topo dir
    DocPx(ox, oy, px, 8, 6, 2, 1, p.frame);   // ponte central
    DocPx(ox, oy, px, 4, 6, 4, 3, p.lens);    // lente esq
    DocPx(ox, oy, px, 10, 6, 4, 3, p.lens);   // lente dir
    DocPx(ox, oy, px, 4, 6, 1, 3, p.frame);   // bordas das lentes
    DocPx(ox, oy, px, 7, 6, 1, 3, p.frame);
    DocPx(ox, oy, px, 10, 6, 1, 3, p.frame);
    DocPx(ox, oy, px, 13, 6, 1, 3, p.frame);

    // Olhos atrás das lentes (piscam). reactT arregala.
    float blinkPhase = fmodf(time, 3.2f);
    bool blink = (blinkPhase > 3.02f);
    if (blink)
    {
        DocPx(ox, oy, px, 5, 7, 2, 1, p.frame); // olhos fechados (linha)
        DocPx(ox, oy, px, 11, 7, 2, 1, p.frame);
    }
    else
    {
        int eyeH = (reactT > 0.5f) ? 2 : 1;     // arregala ao reagir
        DocPx(ox, oy, px, 5, 7 - (eyeH - 1), 2, eyeH, WHITE);
        DocPx(ox, oy, px, 11, 7 - (eyeH - 1), 2, eyeH, WHITE);
        DocPx(ox, oy, px, 6, 7, 1, 1, (Color){ 20, 30, 45, 255 }); // pupilas
        DocPx(ox, oy, px, 11, 7, 1, 1, (Color){ 20, 30, 45, 255 });
    }
    // Sobrancelhas (sobem ao reagir)
    int browY = (reactT > 0.5f) ? 4 : 5;
    DocPx(ox, oy, px, 4, browY, 3, 1, p.hair);
    DocPx(ox, oy, px, 11, browY, 3, 1, p.hair);

    // --- Nariz ---
    DocPx(ox, oy, px, 8, 8, 2, 2, p.skinSh);

    // --- Boca (sorriso largo; abre ao falar) ---
    bool mouthOpen = talking && (sinf(time * 16.0f) > 0.0f);
    if (mouthOpen)
    {
        DocPx(ox, oy, px, 6, 10, 6, 2, p.mouth);
        DocPx(ox, oy, px, 6, 10, 6, 1, WHITE); // dentes
    }
    else
    {
        DocPx(ox, oy, px, 6, 10, 6, 1, p.mouth);   // sorriso fechado
        DocPx(ox, oy, px, 5, 10, 1, 1, p.skinSh);  // covinhas
        DocPx(ox, oy, px, 12, 10, 1, 1, p.skinSh);
    }

    // --- Jaleco / colarinho ---
    DocPx(ox, oy, px, 2, 13, 14, 5, p.coat);
    DocPx(ox, oy, px, 7, 13, 1, 4, p.skinSh);  // gola V
    DocPx(ox, oy, px, 10, 13, 1, 4, p.skinSh);
    DocPx(ox, oy, px, 8, 13, 2, 1, p.skin);    // pescoço
    // Emblema de anticorpo "Y" (acento verde) na lapela.
    DocPx(ox, oy, px, 12, 15, 1, 2, p.accent);
    DocPx(ox, oy, px, 11, 14, 1, 1, p.accent);
    DocPx(ox, oy, px, 13, 14, 1, 1, p.accent);

    // Anel/moldura ciano por cima (acabamento; combina com o HUD do tutorial).
    DrawCircleLines((int)c.x, (int)c.y, radius + 2.0f, DOC_CYAN);
    DrawCircleLines((int)c.x, (int)c.y, radius + 1.0f, Fade(DOC_CYAN, 0.4f));
}
