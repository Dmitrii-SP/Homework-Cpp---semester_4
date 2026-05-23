#pragma once
#include "raylib.h"
#include "widget.hpp"
#include <string>
#include <algorithm>
#include <iostream>

class Button : public Widget
{
private:
    Color mDefaultColor {179, 230, 255, 255};
    Color mHoverColor   {128, 212, 255, 255};
    Color mPressedColor {  0, 136, 204, 255};

    Rectangle mRect {};
    std::string mText;

    int mFontSize {30};
    bool mIsPressed {false};

public:
    Button(Rectangle rect, const std::string& textData)
        : mRect(rect), mText(textData)
    {
        mFontSize = calculateOptimalFontSize();
    }

    void setFontSize(int fontSize)
    {
        mFontSize = fontSize;
    }

    void draw() const override
    {
        DrawRectangleRec(mRect, getCurrentColor());
        int textWidth = MeasureText(mText.c_str(), mFontSize);

        Vector2 position = 
        {
            mRect.x + (mRect.width - textWidth) / 2,
            mRect.y + (mRect.height - mFontSize) / 2.0f
        };

        DrawText(mText.c_str(), (int)position.x, (int)position.y, mFontSize, BLACK);
    }

    void update() override
    {
        Vector2 mouse = GetMousePosition();
        bool isHover = CheckCollisionPointRec(mouse, mRect);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && isHover)
        {
            mIsPressed = true;
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            if (mIsPressed && isHover)
            {
                std::cout << "Button clicked\n";
            }
            mIsPressed = false;
        }
    }

private:
    int calculateOptimalFontSize()
    {
        float widthFraction = 0.6f;
        float heightFraction = 0.4f;

        int baseSize = mFontSize;
        int textWidth = MeasureText(mText.c_str(), baseSize);

        float widthRatio = mRect.width / textWidth * widthFraction;
        float heightRatio = mRect.height / baseSize * heightFraction;

        float minRatio = std::min(widthRatio, heightRatio);

        return (int)(baseSize * minRatio);
    }

    Color getCurrentColor() const
    {
        Vector2 mouse = GetMousePosition();
        bool isHover = CheckCollisionPointRec(mouse, mRect);

        if (mIsPressed)
            return mPressedColor;
        else if (isHover)
            return mHoverColor;
        else
            return mDefaultColor;
    }
};