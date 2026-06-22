#include "raylib.h"
#include <stdio.h>

#define MAP_SIZE 4000
#define BODY_CX 2000.0f

typedef struct BodyPart {
    Vector2 a;
    Vector2 b;
    float radius;
} BodyPart;

static const BodyPart BODY[] = {
    {{BODY_CX, 482.0f}, {BODY_CX, 482.0f}, 315.0f},
    {{BODY_CX, 770.0f}, {BODY_CX, 955.0f}, 165.0f},
    {{1570.0f, 1110.0f}, {2430.0f, 1110.0f}, 255.0f},
    {{1570.0f, 1370.0f}, {2430.0f, 1370.0f}, 250.0f},
    {{1580.0f, 1780.0f}, {2420.0f, 1780.0f}, 330.0f},
    {{1560.0f, 2220.0f}, {2440.0f, 2220.0f}, 315.0f},
    {{BODY_CX, 1260.0f}, {BODY_CX, 2280.0f}, 420.0f},
    {{1510.0f, 1080.0f}, {1300.0f, 1650.0f}, 170.0f},
    {{1300.0f, 1650.0f}, {1140.0f, 2520.0f}, 145.0f},
    {{2490.0f, 1080.0f}, {2700.0f, 1650.0f}, 170.0f},
    {{2700.0f, 1650.0f}, {2860.0f, 2520.0f}, 145.0f},
    {{1740.0f, 2390.0f}, {1680.0f, 2940.0f}, 220.0f},
    {{1680.0f, 2940.0f}, {1645.0f, 3370.0f}, 165.0f},
    {{1645.0f, 3370.0f}, {1625.0f, 3715.0f}, 160.0f},
    {{2260.0f, 2390.0f}, {2320.0f, 2940.0f}, 220.0f},
    {{2320.0f, 2940.0f}, {2355.0f, 3370.0f}, 165.0f},
    {{2355.0f, 3370.0f}, {2375.0f, 3715.0f}, 160.0f},
};

static void DrawBodyPart(Image *image, BodyPart part, float radius, Color color)
{
    ImageDrawLineEx(image, part.a, part.b, (int)(radius * 2.0f), color);
    ImageDrawCircleV(image, part.a, (int)radius, color);
    ImageDrawCircleV(image, part.b, (int)radius, color);
}

int main(int argc, char **argv)
{
    const char *output = argc > 1 ? argv[1] : "mapa_area_jogavel.png";
    const int count = (int)(sizeof(BODY) / sizeof(BODY[0]));
    Image image = GenImageColor(MAP_SIZE, MAP_SIZE, BLANK);

    // Alpha is present only inside the collision geometry used by the game.
    for (int i = 0; i < count; i++) DrawBodyPart(&image, BODY[i], BODY[i].radius, (Color){175, 80, 96, 255});
    for (int i = 0; i < count; i++) DrawBodyPart(&image, BODY[i], BODY[i].radius - 18.0f, (Color){96, 36, 46, 255});
    for (int i = 0; i < count; i++) DrawBodyPart(&image, BODY[i], BODY[i].radius * 0.46f, (Color){112, 44, 55, 255});

    bool ok = ExportImage(image, output);
    UnloadImage(image);
    if (!ok) {
        fprintf(stderr, "Failed to export %s\n", output);
        return 1;
    }
    printf("Exported %s (%dx%d)\n", output, MAP_SIZE, MAP_SIZE);
    return 0;
}
