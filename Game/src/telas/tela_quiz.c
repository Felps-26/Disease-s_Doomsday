#include "../../include/telas.h"
#include <stdio.h>
#include <string.h>

// Perguntas para o Quiz do DF
typedef struct QuizQuestion {
    const char *question;
    const char *options[4];
    int correctOption;
} QuizQuestion;

static QuizQuestion quizDB[] = {
    {
        "Qual o principal vetor da Dengue, Zika e Chikungunya no DF?",
        {"Carrapato-estrela", "Aedes aegypti", "Mosquito-palha", "Barbeiro"},
        1
    },
    {
        "Onde devemos descartar pneus velhos para evitar a Dengue?",
        {"Na rua ou terrenos baldios", "Em ecopontos e locais cobertos", "Nos rios e lagos", "No lixo comum reciclável"},
        1
    },
    {
        "A vacina da Gripe (Influenza) oferecida na UBS deve ser tomada com qual frequencia?",
        {"Apenas uma vez na vida", "A cada 5 anos", "Anualmente", "A cada 10 anos"},
        2
    },
    {
        "Como a Vigilancia Ambiental atua no controle de zoonoses?",
        {"Vacinando criancas nas escolas", "Multando pedestres", "Monitorando vetores e animais transmissores", "Limpando as ruas do centro"},
        2
    }
};

static int currentQuestionIdx = 0;
static bool questionAnswered = false;
static int selectedOption = -1;
static bool quizInitialized = false;

static UIButton quizOptionsBtn[4];

void DrawTelaQuiz(GameState *game, Font font)
{
    if (!quizInitialized) {
        currentQuestionIdx = GetRandomValue(0, 3);
        questionAnswered = false;
        selectedOption = -1;
        
        for (int i = 0; i < 4; i++) {
            quizOptionsBtn[i] = (UIButton){ { 240, 300 + i * 70, 800, 50 }, quizDB[currentQuestionIdx].options[i], false, false };
        }
        quizInitialized = true;
    }

    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, THEME_COLOR_BG_DARK, THEME_COLOR_BG_LIGHT);
    
    const char *title = "QUIZ DA VIGILANCIA EPIDEMIOLOGICA";
    Vector2 titleSz = MeasureTextEx(font, title, 32.0f, 1.0f);
    DrawTextEx(font, title, (Vector2){ (SCREEN_WIDTH / 2.0f) - (titleSz.x / 2.0f), 80.0f }, 32.0f, 1.0f, THEME_COLOR_MAIN);

    const char *question = quizDB[currentQuestionIdx].question;
    Vector2 qSz = MeasureTextEx(font, question, 24.0f, 1.0f);
    DrawTextEx(font, question, (Vector2){ (SCREEN_WIDTH / 2.0f) - (qSz.x / 2.0f), 180.0f }, 24.0f, 1.0f, WHITE);

    for (int i = 0; i < 4; i++) {
        Color baseCol = THEME_COLOR_PANEL;
        Color borderCol = THEME_COLOR_BORDER;
        Color textCol = WHITE;

        if (questionAnswered) {
            if (i == quizDB[currentQuestionIdx].correctOption) {
                baseCol = Fade(GREEN, 0.4f);
                borderCol = GREEN;
                textCol = GREEN;
            } else if (i == selectedOption) {
                baseCol = Fade(RED, 0.4f);
                borderCol = RED;
                textCol = RED;
            }
        } else {
            if (quizOptionsBtn[i].hover) {
                baseCol = Fade(THEME_COLOR_BORDER, 0.8f);
                borderCol = THEME_COLOR_MAIN;
                textCol = THEME_COLOR_MAIN;
            }
        }

        DrawRectangleRounded(quizOptionsBtn[i].bounds, 0.2f, 6, baseCol);
        DrawRectangleRoundedLines(quizOptionsBtn[i].bounds, 0.2f, 6, borderCol);

        DrawTextEx(font, quizOptionsBtn[i].text, (Vector2){ quizOptionsBtn[i].bounds.x + 20, quizOptionsBtn[i].bounds.y + 15 }, 20.0f, 1.0f, textCol);
    }

    if (questionAnswered) {
        const char *msg = (selectedOption == quizDB[currentQuestionIdx].correctOption) ? "CORRETO! +50 PONTOS DO SUS" : "INCORRETO! TENTE DE NOVO MAIS TARDE";
        Color msgCol = (selectedOption == quizDB[currentQuestionIdx].correctOption) ? GREEN : RED;
        Vector2 msgSz = MeasureTextEx(font, msg, 28.0f, 1.0f);
        DrawTextEx(font, msg, (Vector2){ (SCREEN_WIDTH / 2.0f) - (msgSz.x / 2.0f), 600.0f }, 28.0f, 1.0f, msgCol);

        const char *cont = "Clique em qualquer lugar para continuar...";
        Vector2 contSz = MeasureTextEx(font, cont, 18.0f, 1.0f);
        DrawTextEx(font, cont, (Vector2){ (SCREEN_WIDTH / 2.0f) - (contSz.x / 2.0f), 650.0f }, 18.0f, 1.0f, GRAY);
    }
}

void UpdateTelaQuiz(GameState *game, Vector2 mouse)
{
    if (questionAnswered) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            quizInitialized = false;
            game->currentScreen = SCREEN_UPGRADE; // Move to upgrade after quiz
        }
        return;
    }

    for (int i = 0; i < 4; i++) {
        if (CheckCollisionPointRec(mouse, quizOptionsBtn[i].bounds)) {
            quizOptionsBtn[i].hover = true;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                selectedOption = i;
                questionAnswered = true;
                if (selectedOption == quizDB[currentQuestionIdx].correctOption) {
                    game->player.susPoints += 50;
                }
            }
        } else {
            quizOptionsBtn[i].hover = false;
        }
    }
}
