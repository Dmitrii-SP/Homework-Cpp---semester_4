#include <iostream>
#include <cmath>
#include <raylib.h>

class Draggable
{
private:
    Rectangle mRect {};
    Color mColor {};
    bool mIsDragged {false};
    Vector2 mOffset {0.0f, 0.0f};

public:
    Draggable(Vector2 position, Vector2 size, Color color) : mColor{color}
    {
        mRect.x = position.x;
        mRect.y = position.y;
        mRect.width = size.x;
        mRect.height = size.y;
    }

    virtual bool onMousePressed(Vector2 mousePosition)
    {
        if (CheckCollisionPointRec(mousePosition, mRect))
        {
            mIsDragged = true;
            mOffset.x = mousePosition.x - mRect.x;
            mOffset.y = mousePosition.y - mRect.y;
        }
        return mIsDragged;
    }

    virtual void onMouseReleased()
    {
        mIsDragged = false;
    }

    void onMouseMoved(Vector2 mousePosition)
    {
        if (mIsDragged)
        {
            mRect.x = mousePosition.x - mOffset.x;
            mRect.y = mousePosition.y - mOffset.y;
        }
    }

    void handleEvents()
    {
        Vector2 mousePosition = GetMousePosition();
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            onMousePressed(mousePosition);
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            onMouseReleased();
        }

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) || GetMouseDelta().x != 0 || GetMouseDelta().y != 0)
        {
            onMouseMoved(mousePosition);
        }
    }

    void setColor(Color c) { mColor = c; }
    void draw() const { DrawRectangleRec(mRect, mColor); }
};


class DraggableWithColorChange : public Draggable 
{
private:
    Color mBaseColor;
    Color mDragColor; 

public:
    DraggableWithColorChange(Vector2 position, Vector2 size, Color baseColor, Color dragColor) 
        : Draggable(position, size, baseColor), mBaseColor(baseColor), mDragColor(dragColor) 
    {
    }

    bool onMousePressed(Vector2 mousePosition) override 
    {
        bool isHit = Draggable::onMousePressed(mousePosition);
        
        if (isHit) 
        {
            setColor(mDragColor); 
        }
        return isHit;
    }

    void onMouseReleased() override 
    {
        Draggable::onMouseReleased();
        
        setColor(mBaseColor);
    }
};


int main()
{
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Task 3: Color Change on Drag");
    SetTargetFPS(60);
    DraggableWithColorChange myRect({300, 200}, {150, 100}, GRAY, ORANGE);

    while (!WindowShouldClose())
    {
        myRect.handleEvents();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        
        myRect.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}