#include <iostream>
#include <string>
#include <vector>
#include "raylib.h"

class BaseWindow {
protected:
    Rectangle mBounds; 
    Rectangle mHeader; 

public:
    BaseWindow(Rectangle bounds) : mBounds(bounds) {
        mHeader = {bounds.x, bounds.y, bounds.width, 30.0f};
    }

    virtual void draw() const {
        DrawRectangleRec(mBounds, LIGHTGRAY); 
        DrawRectangleRec(mHeader, DARKGRAY);  
        DrawRectangleLinesEx(mBounds, 2, GRAY);
    }

    virtual ~BaseWindow() = default;
};


class Button
{
private:
    inline static const Color sDefaultColor{220, 220, 220, 255};
    inline static const Color sHoverColor{180, 200, 180, 255};
    inline static const Color sPressedColor{140, 160, 140, 255};

    Rectangle mRect;
    Vector2 mTextPosition;
    std::string mText;
    int mFontSize;
    Color mCurrentColor;
    bool mIsPressed;

public:
    Button(Rectangle rect, int fontSize, const std::string& textData)
        : mRect(rect), mText(textData), mFontSize(fontSize)
    {
        int textWidth = MeasureText(mText.c_str(), mFontSize);
        mTextPosition.x = mRect.x + (mRect.width - textWidth) / 2.0f;
        mTextPosition.y = mRect.y + (mRect.height - mFontSize) / 2.0f;
        mCurrentColor = sDefaultColor;
        mIsPressed = false;
    }

    void draw()
    {
        DrawRectangleRec(mRect, mCurrentColor);
        DrawText(mText.c_str(), (int)mTextPosition.x, (int)mTextPosition.y, mFontSize, BLACK);
    }

    bool handleEvent()
    {
        Vector2 mouse = GetMousePosition();
        bool hovered = CheckCollisionPointRec(mouse, mRect);

        if (!mIsPressed)
            mCurrentColor = hovered ? sHoverColor : sDefaultColor;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hovered)
        {
            mIsPressed = true;
            mCurrentColor = sPressedColor;
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            bool clicked = mIsPressed && hovered;
            mIsPressed = false;
            mCurrentColor = hovered ? sHoverColor : sDefaultColor;
            return clicked;
        }

        return false;
    }
};


class MessageWindow : public BaseWindow {
protected:
    std::string mText;
    Color mBgColor;

public:
    MessageWindow(Rectangle bounds, const std::string& text, Color bgColor = RAYWHITE)
        : BaseWindow(bounds), mText(text), mBgColor(bgColor) {}

    void draw() const override {
        DrawRectangleRec(mBounds, mBgColor);
        DrawRectangleRec(mHeader, DARKGRAY);
        DrawRectangleLinesEx(mBounds, 2, BLACK);
        
        DrawText(mText.c_str(), (int)mBounds.x + 15, (int)mBounds.y + 50, 20, BLACK);
    }
};

class ErrorWindow : public MessageWindow {
public:
    ErrorWindow(Rectangle bounds, const std::string& text)
        : MessageWindow(bounds, text, {255, 200, 200, 255}) {} 
};

class DoneWindow : public MessageWindow {
public:
    DoneWindow(Rectangle bounds, const std::string& text)
        : MessageWindow(bounds, text, {200, 255, 200, 255}) {} 
};

class QuestionWindow : public BaseWindow {
private:
    std::string mText;
    Button mOkBtn;
    Button mCancelBtn;

public:
    QuestionWindow(Rectangle bounds, const std::string& text)
        : BaseWindow(bounds), mText(text),
          mOkBtn({bounds.x + 20, bounds.y + bounds.height - 60, 80, 40}, 18, "Ok"),
          mCancelBtn({bounds.x + bounds.width - 120, bounds.y + bounds.height - 60, 100, 40}, 18, "Cancel") 
    {}

    void handleEvents() {
        if (mOkBtn.handleEvent()) {
            std::cout << "Ok" << std::endl;
        }
        if (mCancelBtn.handleEvent()) {
            std::cout << "Cancel" << std::endl;
        }
    }

    void draw() const override {
        BaseWindow::draw();
        DrawText(mText.c_str(), (int)mBounds.x + 15, (int)mBounds.y + 50, 20, BLACK);
        
        const_cast<Button&>(mOkBtn).draw();
        const_cast<Button&>(mCancelBtn).draw();
    }
};


int main() {
    InitWindow(800, 600, "Task 4: Windows & Buttons");
    SetTargetFPS(60);

    ErrorWindow errWin({50, 50, 300, 150}, "File not found!");
    DoneWindow doneWin({400, 50, 300, 150}, "Download complete!");
    QuestionWindow questWin({250, 300, 300, 200}, "Exit program?");

    while (!WindowShouldClose()) {
        questWin.handleEvents();

        BeginDrawing();
        ClearBackground(WHITE);
        
        errWin.draw();
        doneWin.draw();
        questWin.draw();
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}