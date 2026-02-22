#include "raylib.h"
#include <string>

int main() {
    const int Width = 800;
    const int Height = 450;
    InitWindow(Width, Height, "Mouse Coordinates");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();

        std::string xStr = std::to_string(mouseX);
        std::string yStr = std::to_string(mouseY);
       
        std::string fullText = "Mouse Position: X=" + xStr + " Y=" + yStr;

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawCircle(mouseX, mouseY, 5, RED);
            DrawText(fullText.c_str(), 20, 20, 25, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}