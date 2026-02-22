#include "raylib.h"
#include "raymath.h"

int main() {
    const int Width = 800;
    const int Height = 450;
    InitWindow(Width, Height, "Non-Constant Speed");
    
    Vector2 pos = {Width / 2.0f, Height / 2.0f};
    Vector2 velocity = {0, 0}; 
    float acceleration = 0.3f;   
    float radius = 20.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            Vector2 direction = Vector2Subtract(mousePos, pos);
            
            if (Vector2Length(direction) > 0) {
                direction = Vector2Normalize(direction);
                velocity = Vector2Add(velocity, Vector2Scale(direction, acceleration));
            }
        }

        pos = Vector2Add(pos, velocity);

        if ((pos.x + radius >= Width) || (pos.x - radius <= 0)) {
            velocity.x *= -1.0f; 

            if (pos.x - radius <= 0) pos.x = radius;
            else pos.x = Width - radius;
        }

        if ((pos.y + radius >= Height) || (pos.y - radius <= 0)) {
            velocity.y *= -1.0f; 
            
            if (pos.y - radius <= 0) pos.y = radius;
            else pos.y = Height - radius;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawCircleV(pos, radius, DARKBLUE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}