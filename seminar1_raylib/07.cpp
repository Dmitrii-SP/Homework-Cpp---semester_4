#include "raylib.h"

int main() {
    const int Width = 800;
    const int Height = 450;
    InitWindow(Width, Height, "Control & Random Color");

    Vector2 ballPosition = {(float)Width/2, (float)Height/2};
    Color ballColor = DARKBLUE;
    float ballSpeed = 4.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_RIGHT)) ballPosition.x += ballSpeed;
        if (IsKeyDown(KEY_LEFT)) ballPosition.x -= ballSpeed;
        if (IsKeyDown(KEY_UP)) ballPosition.y -= ballSpeed;
        if (IsKeyDown(KEY_DOWN)) ballPosition.y += ballSpeed;

        if (IsKeyPressed(KEY_SPACE)) {
            ballColor = ColorFromHSV((float)GetRandomValue(0, 360), 0.9f, 0.9f);
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawCircleV(ballPosition, 30, ballColor);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}