#include "raylib.h"
#include <vector>

class Slider {
private:
    Rectangle mBar;      
    Rectangle mHandle;    
    float mValue;          
    bool mIsDragging;

public:
    Slider(Rectangle bar) : mBar(bar), mValue(0.5f), mIsDragging(false) {
        mHandle = { mBar.x + mBar.width * mValue - 5, mBar.y - 5, 10, mBar.height + 10 };
    }

    void handleEvent() {
        Vector2 mouse = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, mHandle)) {
            mIsDragging = true;
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) mIsDragging = false;

        if (mIsDragging) {
            mValue = (mouse.x - mBar.x) / mBar.width;
            if (mValue < 0) mValue = 0;
            if (mValue > 1) mValue = 1;
            mHandle.x = mBar.x + mBar.width * mValue - 5;
        }
    }

    float getValue() const { return mValue; }

    void draw() const {
        DrawRectangleRec(mBar, LIGHTGRAY);  
        DrawRectangleRec(mHandle, BLUE);   
    }
};


int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Task 6: RGB Sliders");
    SetTargetFPS(60);

    Slider redSlider({ 50, 100, 300, 10 });
    Slider greenSlider({ 50, 200, 300, 10 });
    Slider blueSlider({ 50, 300, 300, 10 });

    while (!WindowShouldClose()) {
        redSlider.handleEvent();
        greenSlider.handleEvent();
        blueSlider.handleEvent();

        unsigned char r = (unsigned char)(redSlider.getValue() * 255);
        unsigned char g = (unsigned char)(greenSlider.getValue() * 255);
        unsigned char b = (unsigned char)(blueSlider.getValue() * 255);
        Color circleColor = { r, g, b, 255 };

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Red", 50, 80, 20, RED);
        DrawText("Green", 50, 180, 20, GREEN);
        DrawText("Blue", 50, 280, 20, BLUE);

        redSlider.draw();
        greenSlider.draw();
        blueSlider.draw();

        DrawCircle(600, 225, 80, circleColor);
        DrawCircleLines(600, 225, 80, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}