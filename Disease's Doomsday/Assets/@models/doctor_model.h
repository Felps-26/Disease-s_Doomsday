// doctor_model.h
// Assistente do tutorial: "Doutor Maluco" em pixel art procedural animado.
// Disease's Doomsday — Projeto de Saúde Pública / DF
//
// Módulo isolado (depende só de raylib) para que o personagem possa ser
// previsualizado/iterado fora do jogo e para preparar a adição de novas
// EXPRESSÕES ou PERSONAGENS sem tocar no resto do render.
#ifndef DOCTOR_MODEL_H
#define DOCTOR_MODEL_H

#include "raylib.h"
#include <stdbool.h>

// Estados de expressão previstos (a arquitetura aceita novos sem refatorar o
// chamador). Hoje a animação é derivada de flags (talking/reactT); este enum
// documenta a intenção e serve a futuras expressões/sprite sheets.
typedef enum DoctorExpression {
    DOCTOR_IDLE = 0,
    DOCTOR_TALK,
    DOCTOR_REACT,
    DOCTOR_BLINK,
    DOCTOR_EXPRESSION_COUNT
} DoctorExpression;

// Desenha o doutor encaixado num espaço circular de raio `radius` centrado em
// `center` (espaço de tela). `talking` anima a boca; `reactT` (0..1) ergue
// sobrancelhas/arregala os olhos no início de uma fala. `time` = GetTime().
void DrawTutorialDoctor(Vector2 center, float radius, float time, bool talking, float reactT);

#endif // DOCTOR_MODEL_H
