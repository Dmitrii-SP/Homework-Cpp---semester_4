#include "raylib.h"
#include <cmath> 

int main() {
    const int Width = 800;
    const int Height = 450;
    InitWindow(Width, Height, "Circle Orbit");

    Vector2 center = {Width / 2.0f, Height / 2.0f};
    
    float orbitRadius = 150.0f; 
    float angle = 0.0f;     
    float speed = 2.0f;  

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        angle += speed * GetFrameTime(); 

        float circleX = center.x + orbitRadius * cosf(angle);
        float circleY = center.y + orbitRadius * sinf(angle);

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawCircleV(center, 5, LIGHTGRAY);
            DrawCircleLines(center.x, center.y, orbitRadius, LIGHTGRAY);
            DrawCircle(circleX, circleY, 20, DARKBLUE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}