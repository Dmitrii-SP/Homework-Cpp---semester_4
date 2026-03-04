#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <list>
#include <vector>
#include <cstdlib>
#include <algorithm>

struct Ball {
    Vector2 position;
    float radius;
    bool isChoosen;
    Color color;

    Ball(Vector2 p, float r) : position(p), radius(r), isChoosen(false) {
        color = WHITE;
    }

    void draw() const {
        DrawCircleV(position, radius, color);

        if (isChoosen) {
            const float f = 0.7f;
            DrawLineV({position.x - radius, position.y + radius}, {position.x - radius, position.y + radius*f}, YELLOW);
            DrawLineV({position.x - radius, position.y + radius}, {position.x - radius*f, position.y + radius}, YELLOW);
            DrawLineV({position.x + radius, position.y + radius}, {position.x + radius, position.y + radius*f}, YELLOW);
            DrawLineV({position.x + radius, position.y + radius}, {position.x + radius*f, position.y + radius}, YELLOW);
            DrawLineV({position.x + radius, position.y - radius}, {position.x + radius*f, position.y - radius}, YELLOW);
            DrawLineV({position.x + radius, position.y - radius}, {position.x + radius, position.y - radius*f}, YELLOW);
            DrawLineV({position.x - radius, position.y - radius}, {position.x - radius*f, position.y - radius}, YELLOW);
            DrawLineV({position.x - radius, position.y - radius}, {position.x - radius, position.y - radius*f}, YELLOW);
        }
    }
};

Color GetRandomBallColor() {
    return Color{(unsigned char)(rand()%256), (unsigned char)(rand()%256), (unsigned char)(rand()%256), 255};
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(800, 600, "Select, Move, Delete!");
    SetTargetFPS(60);

    std::list<Ball> balls;
    std::list<Ball> clipboard;

    Rectangle selectionRect{0, 0, 0, 0};
    bool isSelecting = false;
    bool isDragging = false;
    Vector2 lastMousePos = {0, 0};
    
    bool showMenu = false;
    Rectangle menuRect = { 0, 0, 150, 140 }; 

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();

        if (showMenu) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, menuRect)) {
                    int item = (int)((mouse.y - menuRect.y) / 28);
                    
                    if (item == 0) { 
                        for (auto it = balls.begin(); it != balls.end(); ) {
                            if (it->isChoosen) it = balls.erase(it); else ++it;
                        }
                    }
                    else if (item == 1) { 
                        balls.emplace_back(mouse, 10.0f + rand() % 30);
                        balls.back().color = GetRandomBallColor();
                    }
                    else if (item == 2) { 
                        for (auto& b : balls) if (b.isChoosen) b.color = GetRandomBallColor();
                    }
                    else if (item == 3) { 
                        for (auto& b : balls) if (b.isChoosen) b.radius *= 1.25f;
                    }
                    else if (item == 4) { 
                        for (auto& b : balls) if (b.isChoosen) b.radius *= 0.75f;
                    }
                    showMenu = false; 
                } else {
                    showMenu = false; 
                }
            }
        } 
  
        else {
            bool mouseOverAnyBall = false;
            for (auto& b : balls) {
                if (CheckCollisionPointCircle(mouse, b.position, b.radius)) {
                    mouseOverAnyBall = true;
                    break;
                }
            }

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                lastMousePos = mouse;
                if (!mouseOverAnyBall) {
                    if (!IsKeyDown(KEY_LEFT_CONTROL)) {
                        for (auto& b : balls) b.isChoosen = false;
                    }
                    if (IsKeyDown(KEY_LEFT_ALT)) {
                        balls.emplace_back(mouse, 10.0f + rand() % 30);
                        balls.back().color = GetRandomBallColor();
                    } else {
                        isSelecting = true;
                        selectionRect = {mouse.x, mouse.y, 0, 0};
                    }
                } else {
                    for (auto& b : balls) {
                        if (CheckCollisionPointCircle(mouse, b.position, b.radius)) {
                            if (IsKeyDown(KEY_LEFT_CONTROL)) b.isChoosen = !b.isChoosen;
                            else {
                                if (!b.isChoosen) {
                                    for (auto& other : balls) other.isChoosen = false;
                                    b.isChoosen = true;
                                }
                                isDragging = true;
                            }
                            break;
                        }
                    }
                }
            }

            if (isDragging) {
                Vector2 delta = Vector2Subtract(mouse, lastMousePos);
                for (auto& b : balls) if (b.isChoosen) b.position = Vector2Add(b.position, delta);
                lastMousePos = mouse;
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) isDragging = false;
            }

            if (isSelecting) {
                selectionRect.width = mouse.x - selectionRect.x;
                selectionRect.height = mouse.y - selectionRect.y;
                Rectangle normRect = selectionRect;
                if (normRect.width < 0) { normRect.x += normRect.width; normRect.width *= -1; }
                if (normRect.height < 0) { normRect.y += normRect.height; normRect.height *= -1; }

                for (auto& b : balls) {
                    bool insideX = (b.position.x - b.radius >= normRect.x) && (b.position.x + b.radius <= normRect.x + normRect.width);
                    bool insideY = (b.position.y - b.radius >= normRect.y) && (b.position.y + b.radius <= normRect.y + normRect.height);
                    if (insideX && insideY) b.isChoosen = true;
                }
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) isSelecting = false;
            }
        }

        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            showMenu = true;
            menuRect.x = mouse.x;
            menuRect.y = mouse.y;
        }

        if (IsKeyPressed(KEY_SPACE)) {
            for (auto& b : balls) if (b.isChoosen) b.color = GetRandomBallColor();
        }
        if (IsKeyPressed(KEY_DELETE)) {
            for (auto it = balls.begin(); it != balls.end(); ) {
                if (it->isChoosen) it = balls.erase(it); else ++it;
            }
        }
        
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C)) {
            clipboard.clear();
            for (auto& b : balls) if (b.isChoosen) clipboard.push_back(b);
        }
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) {
            for (auto& b : balls) b.isChoosen = false;
            for (auto& b : clipboard) {
                balls.push_back(b);
                balls.back().position = Vector2Add(balls.back().position, {20, 20});
                balls.back().isChoosen = true;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        for (const auto& b : balls) b.draw();

        if (isSelecting) {
            DrawRectangleRec(selectionRect, Fade(SKYBLUE, 0.3f));
            DrawRectangleLinesEx(selectionRect, 1, SKYBLUE);
        }
        
        if (showMenu) {
            DrawRectangleRec(menuRect, RAYWHITE);
            DrawRectangleLinesEx(menuRect, 1, DARKGRAY);
            const char* labels[] = { "Delete", "Create", "Random Color", "Increase", "Decrease" };
            for (int i = 0; i < 5; i++) {
                Rectangle itemBound = { menuRect.x, menuRect.y + i * 28, menuRect.width, 28 };
                if (CheckCollisionPointRec(mouse, itemBound)) DrawRectangleRec(itemBound, SKYBLUE);
                DrawText(labels[i], (int)itemBound.x + 5, (int)itemBound.y + 5, 18, BLACK);
                DrawRectangleLinesEx(itemBound, 0.5f, LIGHTGRAY);
            }
        }
        
        DrawText("Right Click for Menu | ALT+Click to Add", 10, 10, 20, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}