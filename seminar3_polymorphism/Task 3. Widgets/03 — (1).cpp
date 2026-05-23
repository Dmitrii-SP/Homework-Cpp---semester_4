#include "raylib.h"
#include <vector>
#include <iostream>

#include "widget.hpp"
#include "button.hpp"
#include "slider.hpp"
#include "draggable.hpp"

int main()
{
    InitWindow(800, 800, "Widgets");
    SetTargetFPS(60);

    std::vector<Widget*> widgets;

    widgets.push_back(new Button(Rectangle{350, 300, 200, 80}, "Cat"));
    widgets.push_back(new Button(Rectangle{200, 450, 100, 80}, "Dog"));
    widgets.push_back(new Button(Rectangle{600, 600, 150, 70}, "Mouse"));

    widgets.push_back(new Slider(Vector2{500, 500}, Vector2{200, 10}, Vector2{20, 40}));
    widgets.push_back(new Slider(Vector2{300, 100}, Vector2{250, 20}, Vector2{30, 60}));

    widgets.push_back(new Draggable(Vector2{550, 100}, Vector2{200, 120}, Color{20, 120, 50, 255}));
    widgets.push_back(new Draggable(Vector2{50, 550}, Vector2{200, 200}, Color{170, 20, 50, 255}));
    widgets.push_back(new Draggable(Vector2{50, 200}, Vector2{100, 100}, Color{100, 20, 150, 255}));

    while (!WindowShouldClose())
    {
        for (auto w : widgets)
        {
            if (w) w->update();
        }

        BeginDrawing();
        ClearBackground(BLACK);

        for (auto w : widgets)
        {
            if (w) w->draw();
        }

        EndDrawing();
    }

    for (auto w : widgets)
    {
        delete w;
    }
    widgets.clear();

    CloseWindow();
    return 0;
}