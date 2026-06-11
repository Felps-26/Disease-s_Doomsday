#include "player_model.h"
#include "weapons_model.h"
#include "../../include/gameplay.h"
#include "raymath.h"
#include "rlgl.h"
#include <math.h>

// ============================================================================
// PALETA DE CORES POR SKIN DO JOGADOR
// 0 = Padrão (cavaleiro branco)  |  1 = Médica  |  2 = Infectada/Biológica
// ============================================================================
typedef struct PlayerSkinPalette
{
    Color armor;
    Color armorDark;
    Color joint;
    Color detail;   // dourado / cruz / espinhos
    Color earTip;
} PlayerSkinPalette;

static PlayerSkinPalette GetPlayerSkinPalette(int skinId)
{
    PlayerSkinPalette p;
    switch (skinId)
    {
        case 1: // MÉDICA: jaleco branco, detalhes verde-saúde e cruz vermelha
            p.armor     = (Color){ 250, 252, 255, 255 };
            p.armorDark = (Color){ 185, 205, 210, 255 };
            p.joint     = (Color){ 60, 80, 85, 255 };
            p.detail    = (Color){ 225, 55, 60, 255 };   // cruz vermelha
            p.earTip    = (Color){ 0, 190, 130, 255 };   // verde-saúde
            break;
        case 2: // INFECTADA: armadura roxa corrompida com brilho tóxico
            p.armor     = (Color){ 110, 70, 150, 255 };
            p.armorDark = (Color){ 60, 35, 90, 255 };
            p.joint     = (Color){ 30, 15, 45, 255 };
            p.detail    = (Color){ 130, 220, 40, 255 };  // verde-ácido
            p.earTip    = (Color){ 130, 220, 40, 255 };
            break;
        default: // PADRÃO
            p.armor     = (Color){ 235, 240, 250, 255 };
            p.armorDark = (Color){ 160, 170, 190, 255 };
            p.joint     = (Color){ 40, 45, 50, 255 };
            p.detail    = (Color){ 230, 180, 50, 255 };  // dourado
            p.earTip    = (Color){ 30, 100, 200, 255 };  // azul
            break;
    }
    return p;
}

// ============================================================================
// MODELO: JOGADOR (Estilo Cavaleiro Coelho)
// ============================================================================
void DrawPlayerModel(Player *player, float size, Color tint, float time, float attackAnimTimer)
{
    Vector2 pPos = player->position;
    
    // Animação
    float walkCycle = player->isMoving ? time * 12.0f : 0.0f;
    float bob = player->isMoving ? fabs(sinf(walkCycle)) * (size * 0.1f) : sinf(time * 3.0f) * (size * 0.03f); // Idle bobbing

    // Direção (-1 esq, 1 dir)
    int dir = (player->facingDir != 0) ? player->facingDir : 1;
    
    // Cores baseadas na imagem anexada
    rlPushMatrix();
    rlTranslatef(pPos.x, pPos.y, 0.0f);
    rlScalef(player->squashX, player->squashY, 1.0f);
    rlTranslatef(-pPos.x, -pPos.y, 0.0f);
    
    // Cores definidas pela skin selecionada do jogador
    PlayerSkinPalette pal = GetPlayerSkinPalette(player->skinId);
    Color colorArmor = pal.armor;          // Base da armadura
    Color colorArmorDark = pal.armorDark;  // Sombra da armadura
    Color colorJoint = pal.joint;          // Juntas escuras
    Color colorVisor = (player->skinId == 2) ? (Color){ 130, 220, 40, 255 } : BLACK; // Visor brilha na skin infectada
    Color colorDetail = pal.detail;        // Detalhes (fivela, cruz, espinhos)
    Color colorBlueTip = pal.earTip;       // Ponta da orelha

    // Líquido da espada-seringa segue a skin da arma equipada
    Color swordLiquid = WeaponSkinPrimary(player->weaponSkinId);

    if (!player->isMoving) {
        // ========================================================
        // MODO IDLE (Front-facing)
        // ========================================================
        
        // 1. Pernas (Lado a lado)
        // Perna Esq
        Rectangle legL = { pPos.x - size*0.25f, pPos.y + size*0.3f + bob, size*0.2f, size*0.35f };
        DrawRectangleRounded(legL, 0.5f, 4, colorArmorDark);
        DrawRectangleRounded((Rectangle){legL.x, legL.y, size*0.15f, size*0.3f}, 0.5f, 4, colorArmor); // Highlight
        // Pé Esq
        DrawRectangleRounded((Rectangle){ legL.x - size*0.05f, legL.y + size*0.25f, size*0.25f, size*0.1f }, 0.5f, 4, colorArmor);
        
        // Perna Dir
        Rectangle legR = { pPos.x + size*0.05f, pPos.y + size*0.3f + bob, size*0.2f, size*0.35f };
        DrawRectangleRounded(legR, 0.5f, 4, colorArmorDark);
        DrawRectangleRounded((Rectangle){legR.x + size*0.05f, legR.y, size*0.15f, size*0.3f}, 0.5f, 4, colorArmor); // Highlight
        // Pé Dir
        DrawRectangleRounded((Rectangle){ legR.x, legR.y + size*0.25f, size*0.25f, size*0.1f }, 0.5f, 4, colorArmor);

        // 2. Braços
        // Braço Esq (mão direita do personagem, lado esquerdo da tela)
        Vector2 armLStart = { pPos.x - size*0.35f, pPos.y - size*0.05f + bob };
        Vector2 armLHand = { pPos.x - size*0.35f, pPos.y + size*0.2f + bob };
        float swordAngle = 0.0f; // apontando para cima

        if (attackAnimTimer > 0.0f) {
            float animT = attackAnimTimer / 0.22f; // de 1.0 a 0.0
            // Mão levanta para fazer o slash em arco por cima da cabeça
            armLHand.x = pPos.x - size*0.4f + (1.0f - animT) * size*0.8f;
            armLHand.y = pPos.y - size*0.2f - sinf((1.0f - animT) * PI) * size*0.4f;
            swordAngle = dir == 1 ? (-90.0f + (1.0f - animT) * 180.0f) : (90.0f - (1.0f - animT) * 180.0f);
        }

        DrawLineEx(armLStart, armLHand, size*0.12f, colorArmorDark);
        DrawCircleV(armLStart, size*0.15f, colorArmorDark); // Ombro
        DrawSyringeSword(armLHand, size*0.5f, swordAngle, swordLiquid); // Espada na frente do ombro
        DrawCircleV(armLHand, size*0.08f, colorArmor); // Mão segura a espada
        
        // Braço Dir (mão esquerda do personagem, lado direito da tela)
        Vector2 armRStart = { pPos.x + size*0.35f, pPos.y - size*0.05f + bob };
        Vector2 armRHand = { pPos.x + size*0.35f, pPos.y + size*0.2f + bob };
        DrawLineEx(armRStart, armRHand, size*0.12f, colorArmorDark);
        DrawCircleV(armRHand, size*0.08f, colorArmor); // Mão
        DrawCircleV(armRStart, size*0.15f, colorArmorDark); // Ombro

        // 3. Torso e Cinto
        Rectangle torso = { pPos.x - size*0.3f, pPos.y - size*0.1f + bob, size*0.6f, size*0.45f };
        DrawRectangleRounded(torso, 0.4f, 4, colorArmor);
        // Peitoral detalhes (removidos os círculos que pareciam um sutiã)
        // Linha divisória da armadura
        DrawLineEx((Vector2){pPos.x - size*0.25f, pPos.y + size*0.05f + bob}, 
                   (Vector2){pPos.x + size*0.25f, pPos.y + size*0.05f + bob}, size*0.02f, colorArmorDark);
        // Cinto
        DrawRectangle(torso.x, torso.y + torso.height - size*0.1f, torso.width, size*0.08f, colorJoint);
        DrawCircle(pPos.x, torso.y + torso.height - size*0.06f, size*0.05f, colorDetail); // Fivela

        // Detalhe exclusivo por skin (peito)
        if (player->skinId == 1) {
            // Cruz médica vermelha no peitoral
            DrawRectangle(pPos.x - size*0.03f, torso.y + size*0.06f, size*0.06f, size*0.2f, colorDetail);
            DrawRectangle(pPos.x - size*0.1f, torso.y + size*0.13f, size*0.2f, size*0.06f, colorDetail);
        } else if (player->skinId == 2) {
            // Pústulas tóxicas pulsantes na armadura corrompida
            float pulse = 0.8f + sinf(time * 6.0f) * 0.2f;
            DrawCircle(pPos.x - size*0.15f, torso.y + size*0.12f, size*0.05f * pulse, colorDetail);
            DrawCircle(pPos.x + size*0.12f, torso.y + size*0.2f, size*0.04f * pulse, colorDetail);
            DrawCircle(pPos.x + size*0.02f, torso.y + size*0.3f, size*0.03f * pulse, colorDetail);
        }

        // 4. Cabeça e Elmo
        Vector2 headPos = { pPos.x, pPos.y - size*0.35f + bob };
        DrawRectangleRounded((Rectangle){ headPos.x - size*0.25f, headPos.y - size*0.25f, size*0.5f, size*0.5f }, 0.5f, 4, colorArmor);
        // Visor T
        DrawRectangle(headPos.x - size*0.1f, headPos.y - size*0.05f, size*0.2f, size*0.08f, colorVisor); // Linha horizontal
        DrawRectangle(headPos.x - size*0.04f, headPos.y - size*0.05f, size*0.08f, size*0.2f, colorVisor); // Linha vertical

        // Orelhas de coelho
        // Esq
        Vector2 earLStart = { headPos.x - size*0.12f, headPos.y - size*0.2f };
        Vector2 earLEnd = { earLStart.x - size*0.15f, earLStart.y - size*0.35f };
        DrawLineEx(earLStart, earLEnd, size*0.12f, colorArmor);
        DrawLineEx((Vector2){earLStart.x+size*0.02f, earLStart.y}, (Vector2){earLEnd.x+size*0.02f, earLEnd.y}, size*0.04f, WHITE); // brilho
        // Ponta Y azul
        Vector2 yL1 = { earLEnd.x - size*0.06f, earLEnd.y - size*0.08f };
        Vector2 yL2 = { earLEnd.x + size*0.06f, earLEnd.y - size*0.08f };
        DrawLineEx(earLEnd, yL1, size*0.06f, colorBlueTip);
        DrawLineEx(earLEnd, yL2, size*0.06f, colorBlueTip);
        
        // Dir
        Vector2 earRStart = { headPos.x + size*0.12f, headPos.y - size*0.2f };
        Vector2 earREnd = { earRStart.x + size*0.15f, earRStart.y - size*0.35f };
        DrawLineEx(earRStart, earREnd, size*0.12f, colorArmor);
        DrawLineEx((Vector2){earRStart.x-size*0.02f, earRStart.y}, (Vector2){earREnd.x-size*0.02f, earREnd.y}, size*0.04f, WHITE); // brilho
        // Ponta Y azul
        Vector2 yR1 = { earREnd.x - size*0.06f, earREnd.y - size*0.08f };
        Vector2 yR2 = { earREnd.x + size*0.06f, earREnd.y - size*0.08f };
        DrawLineEx(earREnd, yR1, size*0.06f, colorBlueTip);
        DrawLineEx(earREnd, yR2, size*0.06f, colorBlueTip);

    } else {
        // ========================================================
        // MODO MOVIMENTO (Side-facing)
        // ========================================================

    // 1. Braço de Trás
    float armBackAngle = sinf(walkCycle) * 0.5f;
    Vector2 armBackStart = { pPos.x - dir*size*0.1f, pPos.y - size*0.1f + bob };
    Vector2 armBackElbow = { armBackStart.x + sinf(armBackAngle)*size*0.2f, armBackStart.y + cosf(armBackAngle)*size*0.2f };
    Vector2 armBackHand = { armBackElbow.x + sinf(armBackAngle - 0.2f*dir)*size*0.2f, armBackElbow.y + cosf(armBackAngle - 0.2f*dir)*size*0.2f };
    DrawLineEx(armBackStart, armBackElbow, size*0.1f, colorArmorDark);
    DrawLineEx(armBackElbow, armBackHand, size*0.08f, colorArmorDark);
    DrawCircleV(armBackHand, size*0.06f, colorArmorDark); // Mão de trás

        // Perna de Trás
        float legBackAngle = sinf(walkCycle + PI) * 0.5f;
        Vector2 legBackStart = { pPos.x - dir*size*0.1f, pPos.y + size*0.2f + bob };
        Vector2 legBackKnee = { legBackStart.x + sinf(legBackAngle)*size*0.25f, legBackStart.y + cosf(legBackAngle)*size*0.25f };
        Vector2 legBackFoot = { legBackKnee.x + sinf(legBackAngle + 0.2f*dir)*size*0.25f, legBackKnee.y + cosf(legBackAngle + 0.2f*dir)*size*0.25f };
        DrawLineEx(legBackStart, legBackKnee, size*0.15f, colorArmorDark);
        DrawCircleV(legBackKnee, size*0.07f, colorJoint);
        DrawLineEx(legBackKnee, legBackFoot, size*0.12f, colorArmorDark);

        // 2. Torso
        Rectangle torso = { pPos.x - size*0.2f, pPos.y - size*0.15f + bob, size*0.4f, size*0.45f };
        DrawRectangleRounded(torso, 0.4f, 4, colorArmor);
        // Cinto (agora visível de lado)
        DrawRectangle(torso.x, torso.y + torso.height - size*0.1f, torso.width, size*0.08f, colorJoint);
        DrawCircle(pPos.x + dir*size*0.15f, torso.y + torso.height - size*0.06f, size*0.04f, colorDetail); // Fivela lateral

        // Detalhe exclusivo por skin (lateral)
        if (player->skinId == 1) {
            // Cruz médica vermelha lateral
            DrawRectangle(pPos.x - size*0.025f, torso.y + size*0.08f, size*0.05f, size*0.16f, colorDetail);
            DrawRectangle(pPos.x - size*0.08f, torso.y + size*0.135f, size*0.16f, size*0.05f, colorDetail);
        } else if (player->skinId == 2) {
            float pulse = 0.8f + sinf(time * 6.0f) * 0.2f;
            DrawCircle(pPos.x - dir*size*0.08f, torso.y + size*0.14f, size*0.045f * pulse, colorDetail);
            DrawCircle(pPos.x + dir*size*0.05f, torso.y + size*0.26f, size*0.035f * pulse, colorDetail);
        }

        // Perna da Frente
        float legFrontAngle = sinf(walkCycle) * 0.5f;
        Vector2 legFrontStart = { pPos.x + dir*size*0.1f, pPos.y + size*0.2f + bob };
        Vector2 legFrontKnee = { legFrontStart.x + sinf(legFrontAngle)*size*0.25f, legFrontStart.y + cosf(legFrontAngle)*size*0.25f };
        Vector2 legFrontFoot = { legFrontKnee.x + sinf(legFrontAngle + 0.2f*dir)*size*0.25f, legFrontKnee.y + cosf(legFrontAngle + 0.2f*dir)*size*0.25f };
        DrawLineEx(legFrontStart, legFrontKnee, size*0.16f, colorArmor);
        DrawCircleV(legFrontKnee, size*0.08f, colorJoint);
        DrawLineEx(legFrontKnee, legFrontFoot, size*0.14f, colorArmor);

        // 3. Cabeça / Elmo
        Vector2 headPos = { pPos.x + dir * size*0.05f, pPos.y - size*0.35f + bob };
        DrawRectangleRounded((Rectangle){ headPos.x - size*0.2f, headPos.y - size*0.25f, size*0.4f, size*0.5f }, 0.5f, 4, colorArmor);
        
        // Visor
        Vector2 visorPos = { headPos.x + dir * size*0.1f, headPos.y - size*0.05f };
        DrawRectangle(visorPos.x - (dir==1 ? 0 : size*0.15f), visorPos.y, size*0.15f, size*0.08f, colorVisor);
        DrawRectangle(visorPos.x + dir*size*0.05f - (dir==1 ? 0 : size*0.05f), visorPos.y - size*0.05f, size*0.05f, size*0.15f, colorVisor);

        // Orelhas de coelho (Side-facing)
        // Orelha trás
        Vector2 earBStart = { headPos.x - dir*size*0.05f, headPos.y - size*0.2f };
        Vector2 earBEnd = { earBStart.x - dir*size*0.1f, earBStart.y - size*0.3f };
        DrawLineEx(earBStart, earBEnd, size*0.1f, colorArmorDark);
        // Ponta Y
        Vector2 yB1 = { earBEnd.x - size*0.05f, earBEnd.y - size*0.07f };
        Vector2 yB2 = { earBEnd.x + size*0.05f, earBEnd.y - size*0.07f };
        DrawLineEx(earBEnd, yB1, size*0.05f, colorBlueTip);
        DrawLineEx(earBEnd, yB2, size*0.05f, colorBlueTip);

        // Orelha frente
        Vector2 earFStart = { headPos.x + dir*size*0.05f, headPos.y - size*0.2f };
        Vector2 earFEnd = { earFStart.x - dir*size*0.05f, earFStart.y - size*0.35f };
        DrawLineEx(earFStart, earFEnd, size*0.12f, colorArmor);
        // Ponta Y
        Vector2 yF1 = { earFEnd.x - size*0.06f, earFEnd.y - size*0.08f };
        Vector2 yF2 = { earFEnd.x + size*0.06f, earFEnd.y - size*0.08f };
        DrawLineEx(earFEnd, yF1, size*0.06f, colorBlueTip);
        DrawLineEx(earFEnd, yF2, size*0.06f, colorBlueTip);

        // 4. Braço da Frente
        float armFrontAngle = sinf(walkCycle + PI) * 0.5f;
        Vector2 armFrontStart = { pPos.x + dir * size*0.15f, pPos.y - size*0.1f + bob };
        Vector2 armFrontElbow = { armFrontStart.x + sinf(armFrontAngle)*size*0.2f, armFrontStart.y + cosf(armFrontAngle)*size*0.2f };
        Vector2 armFrontHand = { armFrontElbow.x + sinf(armFrontAngle - 0.2f*dir)*size*0.2f, armFrontElbow.y + cosf(armFrontAngle - 0.2f*dir)*size*0.2f };
        
        float swordAngle = dir == 1 ? 45.0f : -45.0f; // Apontada pra cima na diagonal

        if (attackAnimTimer > 0.0f) {
            float animT = attackAnimTimer / 0.22f; // de 1.0 a 0.0
            armFrontHand.x = pPos.x + dir * size*0.4f;
            armFrontHand.y = pPos.y - size*0.4f + (1.0f - animT) * size*0.8f; // braço desce
            armFrontElbow = armFrontStart; // Simplifica o braço durante ataque
            swordAngle = dir == 1 ? (-45.0f + (1.0f - animT) * 180.0f) : (45.0f - (1.0f - animT) * 180.0f);
        }
        
        DrawLineEx(armFrontStart, armFrontElbow, size*0.12f, colorArmorDark);
        DrawLineEx(armFrontElbow, armFrontHand, size*0.1f, colorArmorDark);
        DrawCircleV(armFrontStart, size*0.15f, colorArmorDark); // Ombro
        DrawSyringeSword(armFrontHand, size*0.5f, swordAngle, swordLiquid);
        DrawCircleV(armFrontHand, size*0.08f, colorArmor); // Mão da frente
        
        // Ombro frente
        DrawCircleV(armFrontStart, size*0.15f, colorArmorDark);
    }
    
    rlPopMatrix();
}
