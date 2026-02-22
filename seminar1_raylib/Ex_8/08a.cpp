#include "raylib.h"
#include "raymath.h"

int main() {
    InitWindow(800, 450, "Constant Speed");
    
    Vector2 ballPos = {400, 225};
    float speed = 3.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            Vector2 direction = Vector2Subtract(mousePos, ballPos);
            
            if (Vector2Length(direction) > speed) {
                direction = Vector2Normalize(direction);
                ballPos = Vector2Add(ballPos, Vector2Scale(direction, speed));
            }
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawCircleV(ballPos, 20, MAROON);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}