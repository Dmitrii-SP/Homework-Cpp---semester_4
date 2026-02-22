#include "raylib.h"
#include <vector>

struct MyCircle {
    Vector2 position;
    Color color;
};

int main() {
    const int Width = 800;
    const int Height = 450;
    InitWindow(Width, Height, "Task 9: Circle Manager");

    std::vector<MyCircle> circles;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            MyCircle newCircle;
            newCircle.position = GetMousePosition();
            newCircle.color = WHITE; 
            circles.push_back(newCircle);
        }

        if (IsKeyPressed(KEY_SPACE)) {
            for (auto &c : circles) {
                c.color = ColorFromHSV((float)GetRandomValue(0, 360), 0.8f, 0.9f);
            }
        }

        BeginDrawing();
            ClearBackground(DARKGRAY);
            for (const auto &c : circles) {
                DrawCircleV(c.position, 25.0f, c.color);
                DrawCircleLines(c.position.x, c.position.y, 25.0f, BLACK);
            }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}