#include "raylib.h"

int main() {
    const int Width = 800;
    const int Height = 450;
    InitWindow(Width, Height, "Mouse Collision");

    Rectangle targetRect = { 
        Width / 2.0f - 100, 
        Height / 2.0f - 50, 
        200, 
        100 
    };

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        Vector2 mousePoint = GetMousePosition();
        Color rectColor = GREEN; 

        if (CheckCollisionPointRec(mousePoint, targetRect)) {
            rectColor = RED;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawRectangleRec(targetRect, rectColor);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}