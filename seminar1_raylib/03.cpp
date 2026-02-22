#include "raylib.h"

int main() {
    const int Width = 800;
    const int Height = 450;
    InitWindow(Width, Height, "Rotating Square");

    Rectangle rec = { Width / 2.0f, Height / 2.0f, 100.0f, 100.0f };
    Vector2 origin = {0.0f, 0.0f}; 
    float rotation = 0.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        rotation += 1.5f;

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawRectanglePro(rec, origin, rotation, BLUE);
            DrawCircle(rec.x, rec.y, 3, RED);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}