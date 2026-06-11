#include "weapons_model.h"
#include "rlgl.h"
#include "raymath.h"

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

// ============================================================================
// MODELO: ARMA SEGURADA POR TIPO (1=Lâmina, 2=Fuzil, 3=Granada, 4=BFG)
// As cores da skin (primary/secondary) sao aplicadas em TODOS os modelos.
// Desenhado no frame local: origem na mao, "para cima" = -Y.
// ============================================================================
void DrawHeldWeapon(int weapon, Vector2 handPos, float size, float rotationDeg, Color primary, Color secondary)
{
    // Arma 1: a Espada-Seringa ja existente (o liquido segue a skin).
    if (weapon <= 1)
    {
        DrawSyringeSword(handPos, size, rotationDeg, primary);
        return;
    }

    rlPushMatrix();
    rlTranslatef(handPos.x, handPos.y, 0.0f);
    rlRotatef(rotationDeg, 0.0f, 0.0f, 1.0f);
    float s = size;

    if (weapon == 2)
    {
        // ---- FUZIL CELULA-T: coronha, corpo, cano longo e boca brilhante ----
        Color body   = (Color){ 74, 82, 96, 255 };
        Color bodyDk = (Color){ 40, 45, 56, 255 };
        // Coronha / empunhadura (para baixo, atras da mao)
        DrawRectangleRounded((Rectangle){ -s*0.20f, -s*0.05f, s*0.40f, s*0.72f }, 0.45f, 6, bodyDk);
        // Corpo / receiver
        DrawRectangleRounded((Rectangle){ -s*0.24f, -s*0.62f, s*0.48f, s*0.62f }, 0.30f, 6, body);
        DrawRectangleRoundedLines((Rectangle){ -s*0.24f, -s*0.62f, s*0.48f, s*0.62f }, 0.30f, 6, bodyDk);
        // Mira superior
        DrawRectangle((int)(-s*0.04f), (int)(-s*0.74f), (int)(s*0.08f), (int)(s*0.14f), bodyDk);
        // Cano longo
        DrawRectangle((int)(-s*0.11f), (int)(-s*1.75f), (int)(s*0.22f), (int)(s*1.18f), bodyDk);
        // Listra/energia da skin no cano
        DrawRectangle((int)(-s*0.04f), (int)(-s*1.72f), (int)(s*0.08f), (int)(s*1.10f), primary);
        // Boca / muzzle brilhante (cores da skin)
        DrawCircleV((Vector2){ 0, -s*1.78f }, s*0.17f, primary);
        DrawCircleV((Vector2){ 0, -s*1.78f }, s*0.09f, secondary);
        // Carregador (energia celular)
        DrawRectangleRounded((Rectangle){ -s*0.12f, -s*0.02f, s*0.24f, s*0.5f }, 0.35f, 6, primary);
        DrawRectangleRoundedLines((Rectangle){ -s*0.12f, -s*0.02f, s*0.24f, s*0.5f }, 0.35f, 6, secondary);
    }
    else if (weapon == 3)
    {
        // ---- GRANADA MACROFAGO: tubo grosso e granada redonda na boca ----
        Color body   = (Color){ 86, 74, 56, 255 };
        Color bodyDk = (Color){ 52, 44, 32, 255 };
        // Empunhadura
        DrawRectangleRounded((Rectangle){ -s*0.17f, 0.0f, s*0.34f, s*0.62f }, 0.45f, 6, bodyDk);
        // Tubo curto e grosso
        DrawRectangleRounded((Rectangle){ -s*0.30f, -s*1.15f, s*0.60f, s*1.20f }, 0.40f, 8, body);
        DrawRectangleRoundedLines((Rectangle){ -s*0.30f, -s*1.15f, s*0.60f, s*1.20f }, 0.40f, 8, bodyDk);
        // Tambor
        DrawCircleV((Vector2){ 0, -s*0.45f }, s*0.24f, bodyDk);
        DrawCircleLines(0, (int)(-s*0.45f), s*0.24f, body);
        // Boca larga
        DrawCircleV((Vector2){ 0, -s*1.15f }, s*0.34f, bodyDk);
        // Granada-macrofago saindo (cores da skin)
        DrawCircleV((Vector2){ 0, -s*1.34f }, s*0.27f, primary);
        DrawCircleLines(0, (int)(-s*1.34f), s*0.27f, secondary);
        DrawCircleV((Vector2){ -s*0.08f, -s*1.40f }, s*0.07f, secondary); // brilho/pseudópode
    }
    else
    {
        // ---- VACINA BFG: canhao pesado com orbe de energia ----
        Color body   = (Color){ 58, 70, 82, 255 };
        Color bodyDk = (Color){ 32, 40, 50, 255 };
        // Empunhadura
        DrawRectangleRounded((Rectangle){ -s*0.22f, 0.0f, s*0.44f, s*0.62f }, 0.45f, 6, bodyDk);
        // Corpo grande
        DrawRectangleRounded((Rectangle){ -s*0.42f, -s*1.0f, s*0.84f, s*1.10f }, 0.30f, 8, body);
        DrawRectangleRoundedLines((Rectangle){ -s*0.42f, -s*1.0f, s*0.84f, s*1.10f }, 0.30f, 8, bodyDk);
        // Dois canos / prongs
        DrawRectangle((int)(-s*0.40f), (int)(-s*1.72f), (int)(s*0.20f), (int)(s*0.80f), bodyDk);
        DrawRectangle((int)( s*0.20f), (int)(-s*1.72f), (int)(s*0.20f), (int)(s*0.80f), bodyDk);
        // Orbe de energia (glow nas cores da skin)
        DrawCircleGradient(0, (int)(-s*1.5f), s*0.52f, primary, BLANK);
        DrawCircleV((Vector2){ 0, -s*1.5f }, s*0.27f, primary);
        DrawCircleV((Vector2){ 0, -s*1.5f }, s*0.14f, secondary);
        DrawCircleLines(0, (int)(-s*1.5f), s*0.52f, secondary);
        // Emissor central
        DrawRectangle((int)(-s*0.08f), (int)(-s*1.5f), (int)(s*0.16f), (int)(s*0.55f), secondary);
    }

    rlPopMatrix();
}
