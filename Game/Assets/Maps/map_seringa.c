// map_seringa.c
// Implementação do Mapa: Interior da Seringa de Vacina (Tutorial)
// Disease's Doomsday — Projeto de Saúde Pública / DF
#include "map_seringa.h"
#include "raylib.h"
#include "raymath.h"
#include <math.h>

// ============================================================================
// DrawMapSeringa — Renderiza o interior da seringa (chamar dentro BeginMode2D)
// ============================================================================
void DrawMapSeringa(Font font, int tutorialStep, float time)
{
    // -------------------------------------------------------------------------
    // LÍQUIDO DA VACINA (fundo translúcido azul médico)
    // -------------------------------------------------------------------------
    DrawRectangle(
        (int)SYR_WALL_LEFT, (int)SYR_TOP,
        (int)(SYR_WALL_RIGHT - SYR_WALL_LEFT),
        (int)(SYR_BOTTOM - SYR_TOP),
        Fade((Color){ 140, 200, 240, 255 }, 0.35f)
    );

    // Grade de referência interior (linhas finas translúcidas)
    for (int x = (int)SYR_WALL_LEFT + 80; x < (int)SYR_WALL_RIGHT; x += 80)
        DrawLine(x, (int)SYR_TOP, x, (int)SYR_TAPER_Y, Fade((Color){ 180, 210, 230, 255 }, 0.22f));
    for (int y = (int)SYR_TOP + 80; y < (int)SYR_TAPER_Y; y += 80)
        DrawLine((int)SYR_WALL_LEFT, y, (int)SYR_WALL_RIGHT, y, Fade((Color){ 180, 210, 230, 255 }, 0.22f));

    // -------------------------------------------------------------------------
    // PAREDES LATERAIS DO CILINDRO (bordas plásticas cinza-metal)
    // -------------------------------------------------------------------------
    // Esquerda
    DrawRectangle(0, (int)SYR_TOP,
                  (int)SYR_WALL_LEFT, (int)(SYR_BOTTOM - SYR_TOP),
                  (Color){ 155, 175, 192, 255 });
    // Direita
    DrawRectangle((int)SYR_WALL_RIGHT, (int)SYR_TOP,
                  SYRINGE_WIDTH - (int)SYR_WALL_RIGHT, (int)(SYR_BOTTOM - SYR_TOP),
                  (Color){ 155, 175, 192, 255 });

    // -------------------------------------------------------------------------
    // ÊMBOLO NO TOPO (bloco cinza-escuro + anel de vedação)
    // -------------------------------------------------------------------------
    DrawRectangle(0, 0, SYRINGE_WIDTH, (int)SYR_TOP, (Color){ 128, 148, 162, 255 });
    // Anel de borracha do êmbolo
    DrawRectangle(
        (int)(SYR_WALL_LEFT - 12), (int)(SYR_TOP - 6),
        (int)(SYR_WALL_RIGHT - SYR_WALL_LEFT + 24), 14,
        (Color){ 90, 110, 128, 255 }
    );
    DrawRectangleLinesEx((Rectangle){ 0, 0, SYRINGE_WIDTH, SYR_TOP }, 3.0f,
                          (Color){ 80, 100, 118, 255 });
    // Haste do êmbolo (centro)
    DrawRectangle(SYRINGE_WIDTH / 2 - 10, 0, 20, (int)SYR_TOP,
                  (Color){ 100, 120, 138, 255 });

    // -------------------------------------------------------------------------
    // AFUNILAMENTO INFERIOR (paredes convergentes em direção ao bocal)
    // -------------------------------------------------------------------------
    // Triângulo esquerdo
    DrawTriangle(
        (Vector2){ SYR_WALL_LEFT, SYR_TAPER_Y },
        (Vector2){ 0.0f, SYR_TAPER_Y },
        (Vector2){ (float)SYRINGE_WIDTH / 2.0f - 50.0f, SYR_BOTTOM },
        (Color){ 155, 175, 192, 255 }
    );
    // Triângulo direito
    DrawTriangle(
        (Vector2){ SYR_WALL_RIGHT, SYR_TAPER_Y },
        (Vector2){ (float)SYRINGE_WIDTH / 2.0f + 50.0f, SYR_BOTTOM },
        (Vector2){ (float)SYRINGE_WIDTH, SYR_TAPER_Y },
        (Color){ 155, 175, 192, 255 }
    );

    // -------------------------------------------------------------------------
    // BOCAL / AGULHA (base metálica)
    // -------------------------------------------------------------------------
    DrawRectangle((int)SYR_NEEDLE_X, (int)SYR_BOTTOM,
                  (int)SYR_NEEDLE_W, (int)SYR_NEEDLE_H,
                  (Color){ 175, 195, 210, 255 });
    DrawRectangleLinesEx(
        (Rectangle){ SYR_NEEDLE_X, SYR_BOTTOM, SYR_NEEDLE_W, SYR_NEEDLE_H },
        2.0f, (Color){ 100, 128, 158, 255 }
    );
    // Ponta afunilada da agulha
    DrawTriangle(
        (Vector2){ SYR_NEEDLE_X, SYR_BOTTOM + SYR_NEEDLE_H },
        (Vector2){ SYR_NEEDLE_X + SYR_NEEDLE_W, SYR_BOTTOM + SYR_NEEDLE_H },
        (Vector2){ (float)SYRINGE_WIDTH / 2.0f, SYR_BOTTOM + SYR_NEEDLE_H + 30.0f },
        (Color){ 160, 185, 205, 255 }
    );

    // -------------------------------------------------------------------------
    // GATILHO DE SAÍDA — pisca em verde no passo 2
    // -------------------------------------------------------------------------
    if (tutorialStep == 2)
    {
        float pulse = sinf(time * 5.0f) * 0.35f + 0.65f;
        DrawRectangle(
            (int)SYR_EXIT_X, (int)SYR_EXIT_Y,
            (int)SYR_EXIT_W, (int)SYR_EXIT_H,
            Fade((Color){ 0, 220, 100, 255 }, pulse * 0.42f)
        );
        DrawRectangleLinesEx(
            (Rectangle){ SYR_EXIT_X, SYR_EXIT_Y, SYR_EXIT_W, SYR_EXIT_H },
            2.5f, (Color){ 0, 220, 100, 255 }
        );
        // Seta indicativa
        float arrowX = SYRINGE_WIDTH / 2.0f;
        float arrowY = SYR_EXIT_Y + SYR_EXIT_H / 2.0f;
        DrawTriangle(
            (Vector2){ arrowX - 18, arrowY - 10 },
            (Vector2){ arrowX + 18, arrowY - 10 },
            (Vector2){ arrowX, arrowY + 14 },
            Fade(WHITE, pulse * 0.85f)
        );
        Vector2 saidaSz = MeasureTextEx(font, "SAIDA", 14.0f, 1.0f);
        DrawTextEx(font, "SAIDA",
            (Vector2){ arrowX - saidaSz.x / 2.0f, SYR_EXIT_Y + 6 },
            14.0f, 1.0f, Fade(WHITE, 0.9f));
    }

    // -------------------------------------------------------------------------
    // BORDA GERAL DO CILINDRO (highlight metálico)
    // -------------------------------------------------------------------------
    DrawRectangleLinesEx(
        (Rectangle){ SYR_WALL_LEFT, SYR_TOP,
                     SYR_WALL_RIGHT - SYR_WALL_LEFT, SYR_BOTTOM - SYR_TOP },
        4.0f, (Color){ 95, 125, 155, 255 }
    );

    // Reflexo de luz (linha branca no lado esquerdo do cilindro)
    DrawRectangle((int)SYR_WALL_LEFT + 4, (int)SYR_TOP + 8,
                  6, (int)(SYR_TAPER_Y - SYR_TOP - 16),
                  Fade(WHITE, 0.18f));
}

// ============================================================================
// MapSeringa_CheckExit — Verifica se o jogador está na saída (bocal/agulha)
// ============================================================================
bool MapSeringa_CheckExit(Rectangle playerRect)
{
    Rectangle exitRect = { SYR_EXIT_X, SYR_EXIT_Y, SYR_EXIT_W, SYR_EXIT_H };
    return CheckCollisionRecs(playerRect, exitRect);
}

// ============================================================================
// MapSeringa_ApplyWallCollision — Empurra o jogador para dentro dos limites
// ============================================================================
void MapSeringa_ApplyWallCollision(Vector2 *pos, float r)
{
    float wallTop    = SYR_TOP;
    float wallBottom = SYR_BOTTOM;
    float wallLeft   = SYR_WALL_LEFT;
    float wallRight  = SYR_WALL_RIGHT;

    // Afunilamento: abaixo de SYR_TAPER_Y as paredes convergem
    if (pos->y > SYR_TAPER_Y)
    {
        float t = (pos->y - SYR_TAPER_Y) / (SYRINGE_HEIGHT * 0.35f);
        if (t > 1.0f) t = 1.0f;
        wallLeft  = SYR_WALL_LEFT  + t * (SYRINGE_WIDTH / 2.0f - 50.0f - SYR_WALL_LEFT);
        wallRight = SYR_WALL_RIGHT - t * (SYR_WALL_RIGHT - (SYRINGE_WIDTH / 2.0f + 50.0f));
    }

    if (pos->x < wallLeft  + r) pos->x = wallLeft  + r;
    if (pos->x > wallRight - r) pos->x = wallRight - r;
    if (pos->y < wallTop   + r) pos->y = wallTop   + r;
    if (pos->y > wallBottom - r) pos->y = wallBottom - r;
}
