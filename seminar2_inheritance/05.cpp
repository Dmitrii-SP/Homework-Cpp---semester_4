#include "raylib.h"
#include <vector>

class Toggle {
private:
    Rectangle mBounds;   
    bool mIsOn;          
    float mCircleRadius;  

public:
    Toggle(Rectangle bounds, bool initialState = false) 
        : mBounds(bounds), mIsOn(initialState) {
        mCircleRadius = mBounds.height / 2.0f - 4.0f; 
    }


    void handleEvent() {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(GetMousePosition(), mBounds)) {
                mIsOn = !mIsOn; 
            }
        }
    }

    void draw() const {
        Color bgColor = mIsOn ? LIGHTGRAY : GREEN; 
        DrawRectangleRounded(mBounds, 1.0f, 20, mIsOn ? GREEN : LIGHTGRAY);

        float centerX;
        if (mIsOn) {
            centerX = mBounds.x + mBounds.width - mCircleRadius - 4.0f;
        } else {
            centerX = mBounds.x + mCircleRadius + 4.0f;
        }
        
        float centerY = mBounds.y + mBounds.height / 2.0f;

        DrawCircleV({centerX, centerY}, mCircleRadius, WHITE);
    }

    void setPosition(float x, float y) {
        mBounds.x = x;
        mBounds.y = y;
    }
};

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Task 5: 10 Toggles");
    SetTargetFPS(60);

    std::vector<Toggle> toggles;
    for (int i = 0; i < 10; i++) {
        toggles.push_back(Toggle({0, 0, 80, 40}));
    }

    while (!WindowShouldClose()) {
        for (auto& t : toggles) {
            t.handleEvent();
        }

        float startY = GetScreenHeight() / 10.0f;
        float spacing = GetScreenHeight() / 15.0f;
        for (int i = 0; i < toggles.size(); i++) {
            float x = GetScreenWidth() / 2.0f - 40.0f; 
            float y = startY + i * (40.0f + 10.0f); 
            toggles[i].setPosition(x, y);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (const auto& t : toggles) {
            t.draw();
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}