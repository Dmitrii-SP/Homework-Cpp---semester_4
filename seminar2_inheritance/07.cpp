#include "raylib.h"
#include <vector>
#include <string>

class DropList {
private:
    Rectangle mMainRect;         
    std::vector<std::string> mItems; 
    int mSelectedIndex;       
    bool mIsOpen;              
    float mItemHeight;           

public:
    DropList(Rectangle rect, std::vector<std::string> items) 
        : mMainRect(rect), mItems(items), mSelectedIndex(0), mIsOpen(false) {
        mItemHeight = rect.height;
    }

    void handleEvent() {
        Vector2 mouse = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (!mIsOpen && CheckCollisionPointRec(mouse, mMainRect)) {
                mIsOpen = true;
            } 
            else if (mIsOpen) {
                for (int i = 0; i < mItems.size(); i++) {
                    Rectangle itemRect = { mMainRect.x, mMainRect.y + i * mItemHeight, mMainRect.width, mItemHeight };
                    if (CheckCollisionPointRec(mouse, itemRect)) {
                        mSelectedIndex = i;
                        break;
                    }
                }
                mIsOpen = false; 
            }
        }
    }

    int getSelectedIndex() const { return mSelectedIndex; }

    void draw() const {
        DrawRectangleRec(mMainRect, LIGHTGRAY);
        DrawRectangleLinesEx(mMainRect, 1, DARKGRAY);
        DrawText(mItems[mSelectedIndex].c_str(), mMainRect.x + 10, mMainRect.y + 10, 20, BLACK);
        
        const char* arrow = mIsOpen ? "." : ".";
        DrawText(arrow, mMainRect.x + mMainRect.width - 25, mMainRect.y + 10, 20, BLACK);

        if (mIsOpen) {
            for (int i = 0; i < mItems.size(); i++) {
                Rectangle itemRect = { mMainRect.x, mMainRect.y + i * mItemHeight, mMainRect.width, mItemHeight };
                
                Color color = CheckCollisionPointRec(GetMousePosition(), itemRect) ? SKYBLUE : RAYWHITE;
                
                DrawRectangleRec(itemRect, color);
                DrawRectangleLinesEx(itemRect, 1, DARKGRAY);
                DrawText(mItems[i].c_str(), itemRect.x + 10, itemRect.y + 10, 20, BLACK);
            }
        }
    }
};

int main() {
    InitWindow(800, 600, "Task 7: DropList & Shapes");
    SetTargetFPS(60);

    DropList menu({300, 50, 200, 40}, {"Circle", "Rectangle", "Triangle"});
    // Латиница и символы "▲", "▼" корректно не отображаются

    while (!WindowShouldClose()) {
        menu.handleEvent();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        menu.draw();


        int choice = menu.getSelectedIndex();
        Vector2 center = { 400, 350 };
        
        if (choice == 0) { 
            DrawCircleV(center, 80, RED);
        } 
        else if (choice == 1) { 
            DrawRectangle(center.x - 80, center.y - 80, 160, 160, BLUE);
        } 
        else if (choice == 2) {
            DrawTriangle({center.x, center.y - 90}, {center.x - 90, center.y + 70}, {center.x + 90, center.y + 70}, GREEN);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}