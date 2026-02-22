#include "raylib.h"

int main() {
    const int Width = 800;
    const int Height = 450;
    InitWindow(Width, Height, "Moving Circle");

    float circleX = 0.0f;     
    float circleY = Height / 2.0f; 
    float radius = 40.0f; 
    float speed = 4.0f;     

    SetTargetFPS(60);           

    while (!WindowShouldClose()) {
        circleX += speed;

        if (circleX > Width + radius) {
            circleX = -radius;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawCircleV({circleX, circleY }, radius, MAROON);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}