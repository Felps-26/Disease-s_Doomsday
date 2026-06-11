#include "weapons_model.h"
#include "rlgl.h"

// ============================================================================
// MODELO: ESPADA SERINGA
// ============================================================================
void DrawSyringeSword(Vector2 handPos, float size, float rotationDeg, Color liquidColor)
{
    rlPushMatrix();
    rlTranslatef(handPos.x, handPos.y, 0.0f);
    rlRotatef(rotationDeg, 0.0f, 0.0f, 1.0f);

    float s = size; // scale factor

    Color metal = (Color){ 200, 205, 215, 255 };
    Color metalDark = (Color){ 140, 150, 160, 255 };
    Color glass = Fade((Color){ 150, 220, 255, 255 }, 0.4f);
    Color liquid = liquidColor; // Skin da arma define o líquido da lâmina
    Color handle = (Color){ 180, 80, 20, 255 };
    Color gold = (Color){ 230, 180, 40, 255 };
    
    // Y axis points DOWN in screen coordinates.
    // We want the sword to point UP when rotationDeg = 0.
    // So the Tip (Needle) is at negative Y. The Pommel (Plunger) is at positive Y.
    
    // --- 1. Pommel (Anel do êmbolo no fundo) ---
    DrawCircleLines(0, s*0.6f, s*0.25f, metal);
    DrawCircleLines(0, s*0.6f, s*0.2f, metalDark);
    
    // --- 2. Grip (Cabo marrom onde a mão segura, centrado em 0,0) ---
    DrawRectangle(-s*0.15f, -s*0.35f, s*0.3f, s*0.7f, handle);
    DrawLine(-s*0.15f, -s*0.15f, s*0.15f, -s*0.15f, metalDark);
    DrawLine(-s*0.15f, s*0.15f, s*0.15f, s*0.15f, metalDark);

    // --- 3. Crossguard (Guarda / Flanges da seringa) ---
    DrawRectangle(-s*0.6f, -s*0.5f, s*1.2f, s*0.15f, metal);
    DrawRectangle(-s*0.6f, -s*0.45f, s*1.2f, s*0.05f, metalDark);
    // Pequenos detalhes nas pontas da guarda
    DrawRectangle(-s*0.8f, -s*0.5f, s*0.2f, s*0.05f, metal);
    DrawRectangle(s*0.6f, -s*0.5f, s*0.2f, s*0.05f, metal);
    
    // --- 4. Base Dourada da Lâmina ---
    DrawRectangle(-s*0.35f, -s*0.65f, s*0.7f, s*0.15f, gold);
    
    // --- 5. Blade (Cilindro de Vidro com Líquido) ---
    Rectangle glassRect = { -s*0.3f, -s*1.65f, s*0.6f, s*1.0f };
    Rectangle liquidRect = { -s*0.25f, -s*1.6f, s*0.5f, s*0.75f };
    DrawRectangleRec(liquidRect, liquid);
    // Linhas de medição
    for(int i=0; i<6; i++) {
        float ly = -s*1.55f + (s*0.12f * i);
        DrawLine(-s*0.25f, ly, -s*0.05f, ly, BLACK);
    }
    // Vidro por cima
    DrawRectangleLinesEx(glassRect, s*0.05f, metalDark);
    DrawRectangleRec(glassRect, glass);
    
    // --- 6. Base da Agulha (Topo do cilindro) ---
    DrawRectangle(-s*0.25f, -s*1.85f, s*0.5f, s*0.2f, metalDark);
    DrawRectangle(-s*0.15f, -s*1.95f, s*0.3f, s*0.1f, gold);
    
    // --- 7. Tip (Agulha) ---
    DrawTriangle(
        (Vector2){ -s*0.1f, -s*1.95f },
        (Vector2){ 0, -s*2.95f },
        (Vector2){ s*0.1f, -s*1.95f },
        metal
    );
    // Linha de brilho
    DrawLine(0, -s*1.95f, 0, -s*2.85f, WHITE);
    
    rlPopMatrix();
}
