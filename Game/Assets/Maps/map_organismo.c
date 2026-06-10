// map_organismo.c
// Implementação do Mapa Principal: Interior do Organismo Humano
// Disease's Doomsday — Projeto de Saúde Pública / DF
#include "map_organismo.h"
#include "raylib.h"

// ============================================================================
// DrawMapOrganismo — Renderiza o campo de batalha (chamar dentro BeginMode2D)
// ============================================================================
void DrawMapOrganismo(Font font, bool isBossFight, Color bgColor, Color gridColor)
{
    // -------------------------------------------------------------------------
    // FUNDO DEGRADÊ DO MAPA
    // -------------------------------------------------------------------------
    DrawRectangleGradientV(0, 0, MAP_WIDTH, MAP_HEIGHT, bgColor, bgColor);

    // -------------------------------------------------------------------------
    // GRADE INTERNA (tecido orgânico / células)
    // -------------------------------------------------------------------------
    for (int x = 0; x <= MAP_WIDTH; x += ORG_GRID_SPACING)
        DrawLine(x, 0, x, MAP_HEIGHT, gridColor);
    for (int y = 0; y <= MAP_HEIGHT; y += ORG_GRID_SPACING)
        DrawLine(0, y, MAP_WIDTH, y, gridColor);

    // -------------------------------------------------------------------------
    // DETALHES ORGÂNICOS — círculos que simulam células/vasos sanguíneos
    // -------------------------------------------------------------------------
    // Marcadores de "vasculatura" (estáticos, pseudo-aleatórios por seed)
    static const int cellX[] = { 400, 900, 1500, 2200, 3000, 3600, 600, 1200, 1800, 2600, 3200, 700, 1600, 2400, 3500 };
    static const int cellY[] = { 300, 800, 1200, 600, 900, 1400, 2000, 2500, 1700, 2100, 800, 3000, 3200, 2800, 3500 };
    static const int cellR[] = { 60, 45, 80, 55, 70, 40, 65, 50, 75, 45, 85, 55, 40, 70, 60 };
    int numCells = 15;

    Color cellColor = isBossFight
        ? Fade((Color){ 120, 0, 40, 255 }, 0.18f)   // Boss: veias escuras
        : Fade((Color){ 0, 80, 30, 255 }, 0.18f);   // Normal: células verdes

    for (int i = 0; i < numCells; i++)
    {
        DrawCircleLines(cellX[i], cellY[i], (float)cellR[i], cellColor);
        DrawCircleLines(cellX[i], cellY[i], (float)(cellR[i] / 2), Fade(cellColor, 0.5f));
    }

    // -------------------------------------------------------------------------
    // BORDA DO MAPA (limite do organismo)
    // -------------------------------------------------------------------------
    Color borderColor = isBossFight
        ? (Color){ 160, 0, 30, 255 }     // Boss: borda vermelha de alarme
        : (Color){ 0, 120, 50, 255 };    // Normal: borda verde biológica

    DrawRectangleLinesEx(
        (Rectangle){ 0, 0, MAP_WIDTH, MAP_HEIGHT },
        ORG_BORDER_THICK,
        borderColor
    );

    // -------------------------------------------------------------------------
    // AVISOS DE BORDA (textos de "Zona Infectada")
    // -------------------------------------------------------------------------
    const char *warningText = isBossFight
        ? "!!! INFECCAO CRITICA — LIMITE DO ORGANISMO !!!"
        : "ZONA INFECTADA — LIMITE DO ORGANISMO";

    float warnFontSize = 16.0f;
    Color warnColor = isBossFight
        ? Fade((Color){ 255, 60, 60, 255 }, 0.55f)
        : Fade((Color){ 220, 60, 60, 255 }, 0.40f);

    // Borda superior e inferior
    for (int x = 0; x < MAP_WIDTH; x += ORG_WARNING_STEP)
    {
        DrawTextEx(font, warningText,
            (Vector2){ (float)(x + 20), 22.0f },
            warnFontSize, 1.0f, warnColor);
        DrawTextEx(font, warningText,
            (Vector2){ (float)(x + 20), (float)(MAP_HEIGHT - 40) },
            warnFontSize, 1.0f, warnColor);
    }
    // Borda esquerda e direita (textos rotacionados simulados verticalmente)
    for (int y = 0; y < MAP_HEIGHT; y += ORG_WARNING_STEP)
    {
        DrawTextEx(font, "< ZONA INFECTADA >",
            (Vector2){ 22.0f, (float)(y + 20) },
            14.0f, 1.0f, Fade(warnColor, 0.6f));
        DrawTextEx(font, "< ZONA INFECTADA >",
            (Vector2){ (float)(MAP_WIDTH - 200), (float)(y + 20) },
            14.0f, 1.0f, Fade(warnColor, 0.6f));
    }
}
