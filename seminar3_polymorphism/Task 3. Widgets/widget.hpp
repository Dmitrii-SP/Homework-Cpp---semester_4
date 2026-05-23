#pragma once

class Widget
{
public:
    virtual void update() = 0;   
    virtual void draw() const = 0;
    virtual ~Widget() = default;
};