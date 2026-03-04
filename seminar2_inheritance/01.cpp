#include <iostream>
#include <cmath>
#include <numbers>

struct Vector2f {
    float x, y;
};

Vector2f operator+(Vector2f left, Vector2f right) {
    return {left.x + right.x, left.y + right.y};
}

Vector2f& operator+=(Vector2f& left, Vector2f right) {
    left.x += right.x;
    left.y += right.y;
    return left;
}

std::ostream& operator<<(std::ostream& out, Vector2f pos) {
    out << "(" << pos.x << ", " << pos.y << ")";
    return out;
}

class Shape {
private:
    Vector2f mPosition;

public:
    Shape(Vector2f position) : mPosition(position) {}

    virtual ~Shape() {}

    Vector2f getPosition() const { return mPosition; }

    void setPosition(Vector2f newPosition) { mPosition = newPosition; }

    void move(Vector2f change) { 
        mPosition += change; 
    }
};


class Circle : public Shape {
private:
    float mRadius;
public:
    Circle(float radius, Vector2f pos = {0, 0}) 
        : Shape(pos), mRadius(radius) {}

    float calculateArea() const { 
        return static_cast<float>(M_PI) * mRadius * mRadius;    
    }
};

class Rectangle : public Shape {
private:
    float mWidth, mHeight;
public:
    Rectangle(float width, float height, Vector2f pos = {0, 0}) 
        : Shape(pos), mWidth(width), mHeight(height) {}

    float calculateArea() const { return mWidth * mHeight; }
};

class Triangle : public Shape {
private:
    float mBase, mHeight;
public:
    Triangle(float base, float height, Vector2f pos = {0, 0}) 
        : Shape(pos), mBase(base), mHeight(height) {}

    float calculateArea() const { return 0.5f * mBase * mHeight; }
};

int main() {
    Circle sun(50.0f, {100.0f, 100.0f});
    Rectangle brick(10.0f, 5.0f, {0.0f, 0.0f});
    
    std::cout << "Sun position: " << sun.getPosition() << "\n";
    std::cout << "Brick position: " << brick.getPosition() << "\n";

    std::cout << "\n--- Moving objects ---\n";
    
    sun.move({10.5f, -5.0f});
    brick.move({25.0f, 25.0f});

    std::cout << "New Sun position:" << sun.getPosition() << "\n";
    std::cout << "New Brick position: " << brick.getPosition() << "\n\n";

    return 0;
}