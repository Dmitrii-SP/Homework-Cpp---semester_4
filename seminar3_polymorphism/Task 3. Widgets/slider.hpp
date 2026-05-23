#pragma once
#include "raylib.h"
#include "widget.hpp"
#include <algorithm>

class Slider : public Widget
{
protected:
    Rectangle mTrack {}; 
    Rectangle mThumb {}; 

    bool mIsPressed {false};

public:
    Slider(Vector2 centerPosition, Vector2 trackSize, Vector2 thumbSize)
    {
        mTrack = 
        {
            centerPosition.x - trackSize.x / 2.0f,
            centerPosition.y - trackSize.y / 2.0f,
            trackSize.x,
            trackSize.y
        };

        mThumb = 
        {
            centerPosition.x - thumbSize.x / 2.0f,
            centerPosition.y - thumbSize.y / 2.0f,
            thumbSize.x,
            thumbSize.y
        };
    }

    void draw() const override
    {
        DrawRectangleRec(mTrack, Color{200, 200, 220, 255});
        DrawRectangleRec(mThumb, Color{150, 150, 240, 255});
    }

    void update() override
    {
        Vector2 mouse = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (CheckCollisionPointRec(mouse, mThumb) || CheckCollisionPointRec(mouse, mTrack))
            {
                mIsPressed = true;
                setRestrictedThumbPosition(mouse.x);
            }
        }

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && mIsPressed)
        {
            setRestrictedThumbPosition(mouse.x);
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            mIsPressed = false;
        }
    }

    bool isPressed() const
    {
        return mIsPressed;
    }

    float getValue() const
    {
        float start  = mTrack.x;
        float finish = mTrack.x + mTrack.width;
        float pos    = mThumb.x + mThumb.width / 2.0f;

        return 100.0f * (pos - start) / (finish - start);
    }

private:
    void setRestrictedThumbPosition(float mouseX)
    {
        float min = mTrack.x;
        float max = mTrack.x + mTrack.width;

        float clampedX = std::clamp(mouseX, min, max);
        mThumb.x = clampedX - mThumb.width / 2.0f;
    }
};