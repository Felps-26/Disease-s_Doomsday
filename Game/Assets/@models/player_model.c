#include "player_model.h"
#include "raymath.h"
#include <math.h>

// ============================================================================
// MODELO: JOGADOR
// ============================================================================
void DrawPlayerModel(Player *player, float size, Color tint, float time)
{
    Vector2 pPos = player->position;
    
    // Animação
    float walkCycle = player->isMoving ? time * 12.0f : 0.0f;
    float bob = player->isMoving ? fabs(sinf(walkCycle)) * (size * 0.1f) : sinf(time * 3.0f) * (size * 0.03f); // Idle bobbing

    // Direção (-1 esq, 1 dir)
    int dir = (player->facingDir != 0) ? player->facingDir : 1;
    
    // Cores
    Color colorArmor = tint; // Usa a tintura fornecida
    Color colorJoint = (Color){40, 40, 40, 255};   // Juntas escuras
    Color colorVisor = (Color){10, 15, 20, 255};    // Visor do capacete
    Color colorDetail = (Color){220, 160, 40, 255}; // Detalhes dourados/ombros

    // Membros traseiros (Braço de trás, perna de trás)
    // Perna de trás
    float legBackAngle = player->isMoving ? sinf(walkCycle + PI) * 0.5f : 0.1f;
    Vector2 legBackStart = { pPos.x, pPos.y + size*0.3f + bob };
    Vector2 legBackKnee = { legBackStart.x + sinf(legBackAngle)*size*0.3f, legBackStart.y + cosf(legBackAngle)*size*0.3f };
    Vector2 legBackFoot = { legBackKnee.x + sinf(legBackAngle + 0.2f*dir)*size*0.3f, legBackKnee.y + cosf(legBackAngle + 0.2f*dir)*size*0.3f };
    DrawLineEx(legBackStart, legBackKnee, size*0.12f, colorArmor);
    DrawCircleV(legBackKnee, size*0.06f, colorJoint);
    DrawLineEx(legBackKnee, legBackFoot, size*0.1f, colorArmor);
    
    // Braço de trás
    float armBackAngle = player->isMoving ? sinf(walkCycle) * 0.5f : -0.2f;
    Vector2 armBackStart = { pPos.x, pPos.y - size*0.1f + bob };
    Vector2 armBackElbow = { armBackStart.x + sinf(armBackAngle)*size*0.25f, armBackStart.y + cosf(armBackAngle)*size*0.25f };
    Vector2 armBackHand = { armBackElbow.x + sinf(armBackAngle - 0.2f*dir)*size*0.25f, armBackElbow.y + cosf(armBackAngle - 0.2f*dir)*size*0.25f };
    DrawLineEx(armBackStart, armBackElbow, size*0.1f, colorArmor);
    DrawCircleV(armBackElbow, size*0.05f, colorJoint);
    DrawLineEx(armBackElbow, armBackHand, size*0.08f, colorArmor);

    // Corpo (Torso)
    Rectangle torso = { pPos.x - size*0.2f, pPos.y - size*0.2f + bob, size*0.4f, size*0.5f };
    DrawRectangleRounded(torso, 0.4f, 4, colorArmor);

    // Detalhe cinto/peito
    DrawLineEx((Vector2){pPos.x - size*0.2f, pPos.y + size*0.1f + bob}, 
               (Vector2){pPos.x + size*0.2f, pPos.y + size*0.1f + bob}, size*0.05f, colorDetail);

    // Membros frontais (Braço da frente, perna da frente)
    // Perna da frente
    float legFrontAngle = player->isMoving ? sinf(walkCycle) * 0.5f : -0.1f;
    Vector2 legFrontStart = { pPos.x, pPos.y + size*0.3f + bob };
    Vector2 legFrontKnee = { legFrontStart.x + sinf(legFrontAngle)*size*0.3f, legFrontStart.y + cosf(legFrontAngle)*size*0.3f };
    Vector2 legFrontFoot = { legFrontKnee.x + sinf(legFrontAngle + 0.2f*dir)*size*0.3f, legFrontKnee.y + cosf(legFrontAngle + 0.2f*dir)*size*0.3f };
    DrawLineEx(legFrontStart, legFrontKnee, size*0.15f, colorArmor);
    DrawCircleV(legFrontKnee, size*0.07f, colorJoint);
    DrawLineEx(legFrontKnee, legFrontFoot, size*0.12f, colorArmor);

    // Braço da frente
    float armFrontAngle = player->isMoving ? sinf(walkCycle + PI) * 0.5f : 0.2f;
    Vector2 armFrontStart = { pPos.x + dir * size*0.1f, pPos.y - size*0.1f + bob };
    Vector2 armFrontElbow = { armFrontStart.x + sinf(armFrontAngle)*size*0.25f, armFrontStart.y + cosf(armFrontAngle)*size*0.25f };
    Vector2 armFrontHand = { armFrontElbow.x + sinf(armFrontAngle - 0.2f*dir)*size*0.25f, armFrontElbow.y + cosf(armFrontAngle - 0.2f*dir)*size*0.25f };
    
    // Ombro
    DrawCircleV(armFrontStart, size*0.12f, colorDetail);
    DrawLineEx(armFrontStart, armFrontElbow, size*0.12f, colorArmor);
    DrawCircleV(armFrontElbow, size*0.06f, colorJoint);
    DrawLineEx(armFrontElbow, armFrontHand, size*0.1f, colorArmor);

    // Cabeça / Elmo
    Vector2 headPos = { pPos.x + dir * size*0.05f, pPos.y - size*0.35f + bob*1.2f };
    DrawCircleV(headPos, size*0.25f, colorArmor);

    // Visor (Olho)
    Vector2 visorPos = { headPos.x + dir * size*0.1f, headPos.y - size*0.05f };
    DrawRectangle(visorPos.x - (dir==1 ? 0 : size*0.15f), visorPos.y, size*0.15f, size*0.08f, colorVisor);
    DrawRectangle(visorPos.x + dir*size*0.05f - (dir==1 ? 0 : size*0.05f), visorPos.y - size*0.05f, size*0.05f, size*0.15f, colorVisor); // Cruz T-shape
}
