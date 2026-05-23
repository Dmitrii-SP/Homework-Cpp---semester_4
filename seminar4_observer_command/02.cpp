#include "raylib.h"
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include <cmath>

enum class Event
{
    Rotate,
    ChangeColor
};

class Observer
{
public:
    virtual void update(float value, Event event) = 0;
    virtual ~Observer() = default;
};

class Drawable
{
public:
    virtual void draw() const = 0;
    virtual ~Drawable() = default;
};

class Slider : public Drawable
{
private:
    Rectangle mTrack;
    Rectangle mThumb;
    Color mTrackColor {200, 200, 220, 255};
    Color mThumbColor {150, 150, 240, 255};
    bool mIsPressed {false};
    std::set<Observer*> mObservers;
    Event mEventToTrigger;

public:
    Slider(Vector2 center, Vector2 trackSize, Vector2 thumbSize, Event eventToTrigger)
        : mEventToTrigger(eventToTrigger)
    {
        mTrack = {center.x - trackSize.x / 2, center.y - trackSize.y / 2, trackSize.x, trackSize.y};
        mThumb = {center.x - thumbSize.x / 2, center.y - thumbSize.y / 2, thumbSize.x, thumbSize.y};
    }

    void draw() const override
    {
        DrawRectangleRec(mTrack, mTrackColor);
        DrawRectangleRec(mThumb, mThumbColor);
    }

    void updateInput()
    {
        Vector2 mouse = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (CheckCollisionPointRec(mouse, mThumb) || CheckCollisionPointRec(mouse, mTrack))
            {
                mIsPressed = true;
                mThumb.x = mouse.x - mThumb.width / 2;
                notifyObservers();
            }
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            mIsPressed = false;
        }

        if (mIsPressed)
        {
            float min = mTrack.x;
            float max = mTrack.x + mTrack.width;
            float clampedX = std::clamp(mouse.x, min, max);
            mThumb.x = clampedX - mThumb.width / 2;
            notifyObservers();
        }
    }

    float getValue() const
    {
        float part = (mThumb.x + mThumb.width / 2) - mTrack.x;
        return (part / mTrack.width) * 100.0f;
    }

    void addObserver(Observer* p)
    {
        mObservers.insert(p);
    }

    void notifyObservers()
    {
        for (auto p : mObservers)
        {
            p->update(getValue(), mEventToTrigger);
        }
    }
};

class Circle : public Observer, public Drawable
{
private:
    Vector2 mPosition;
    float mRadius;
    float mRotation {0.0f};
    Color mColor {0, 255, 0, 255};

public:
    Circle(Vector2 position, float radius)
        : mPosition(position), mRadius(radius)
    {
    }

    void draw() const override
    {
        DrawCircleV(mPosition, mRadius, mColor);
        float rad = mRotation * DEG2RAD;
        Vector2 endpoint = {
            mPosition.x + cosf(rad) * mRadius,
            mPosition.y + sinf(rad) * mRadius
        };
        DrawLineV(mPosition, endpoint, BLACK);
    }

    void update(float value, Event event) override
    {
        if (event == Event::Rotate)
        {
            mRotation = 3.6f * value;
        }
        else if (event == Event::ChangeColor)
        {
            float hue = (value / 100.0f) * 360.0f;
            mColor = ColorFromHSV(hue, 1.0f, 1.0f);
        }
    }
};

class Square : public Observer, public Drawable
{
private:
    Vector2 mPosition;
    float mSize;
    float mRotation {0.0f};
    Color mColor {255, 0, 0, 255};

public:
    Square(Vector2 position, float size)
        : mPosition(position), mSize(size)
    {
    }

    void draw() const override
    {
        DrawRectanglePro({mPosition.x, mPosition.y, mSize, mSize}, {mSize / 2, mSize / 2}, mRotation, mColor);
    }

    void update(float value, Event event) override
    {
        if (event == Event::Rotate)
        {
            mRotation = 3.6f * value;
        }
        else if (event == Event::ChangeColor)
        {
            float hue = (value / 100.0f) * 360.0f;
            mColor = ColorFromHSV(hue, 1.0f, 1.0f);
        }
    }
};

int main()
{
    InitWindow(800, 800, "Two Subjects (raylib)");
    SetTargetFPS(60);

    Slider rotateSlider({400, 550}, {500, 20}, {25, 60}, Event::Rotate);
    Slider colorSlider({400, 650}, {500, 20}, {25, 60}, Event::ChangeColor);

    std::vector<Observer*> observers
    {
        new Circle({250, 300}, 60),
        new Square({550, 300}, 100)
    };

    for (auto p : observers)
    {
        rotateSlider.addObserver(p);
        colorSlider.addObserver(p);
    }

    while (!WindowShouldClose())
    {
        rotateSlider.updateInput();
        colorSlider.updateInput();

        BeginDrawing();
        ClearBackground(BLACK);

        rotateSlider.draw();
        colorSlider.draw();

        for (auto p : observers)
        {
            dynamic_cast<Drawable*>(p)->draw();
        }

        EndDrawing();
    }

    for (auto p : observers)
    {
        delete p;
    }

    CloseWindow();
    return 0;
}