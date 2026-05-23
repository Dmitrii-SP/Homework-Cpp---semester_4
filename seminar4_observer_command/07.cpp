#include "raylib.h"
#include <vector>
#include <set>
#include <random>
#include <algorithm>
#include <iostream>
#include <memory>
#include "button.hpp"

float getRandomFloat(float min, float max)
{
    return GetRandomValue(0, 10000) / 10000.0f * (max - min) + min;
}

int getRandomInt(int min, int max)
{
    return GetRandomValue(min, max);
}

Color getRandomColor()
{
    return {(unsigned char)GetRandomValue(0, 255), 
            (unsigned char)GetRandomValue(0, 255), 
            (unsigned char)GetRandomValue(0, 255), 
            255};
}

struct Shape
{
    virtual void draw() = 0;
    virtual void rotate(float a) = 0;
    virtual void setPosition(Vector2 p) = 0;
    virtual Vector2 getPosition() = 0;
    virtual Vector2 getSize() = 0;
    virtual void setColor(Color c) = 0;
    virtual Color getColor() = 0;
    virtual ~Shape() = default;
};

class RectangleShape : public Shape
{
private:
    Vector2 mPosition;
    Vector2 mSize;
    float mRotation {0};
    Color mColor;

public:
    RectangleShape(Vector2 size) : mSize(size) {}

    void draw() override
    {
        DrawRectanglePro({mPosition.x, mPosition.y, mSize.x, mSize.y}, {mSize.x / 2, mSize.y / 2}, mRotation, mColor);
    }

    void rotate(float a) override
    {
        mRotation += a;
    }

    void setPosition(Vector2 p) override
    {
        mPosition = p;
    }

    Vector2 getPosition() override
    {
        return mPosition;
    }

    Vector2 getSize() override
    {
        return mSize;
    }

    void setColor(Color c) override
    {
        mColor = c;
    }

    Color getColor() override
    {
        return mColor;
    }
};

class CircleShape : public Shape
{
private:
    Vector2 mPosition;
    float mRadius;
    float mRotation {0};
    Color mColor;
    int mSegments;

public:
    CircleShape(float r, int seg = 32) : mRadius(r), mSegments(seg) {}

    void draw() override
    {
        DrawCircleV(mPosition, mRadius, mColor);
    }

    void rotate(float a) override
    {
        mRotation += a;
    }

    void setPosition(Vector2 p) override
    {
        mPosition = p;
    }

    Vector2 getPosition() override
    {
        return mPosition;
    }

    Vector2 getSize() override
    {
        return {mRadius * 2, mRadius * 2};
    }

    void setColor(Color c) override
    {
        mColor = c;
    }

    Color getColor() override
    {
        return mColor;
    }
};

class ShapePool
{
private:
    Rectangle mField;
    std::vector<std::unique_ptr<Shape>> mShapes;

public:
    ShapePool(Vector2 position, Vector2 size)
    {
        mField = {position.x, position.y, size.x, size.y};
    }

    void addShape(std::unique_ptr<Shape> p)
    {
        mShapes.push_back(std::move(p));
    }

    void removeLastShape()
    {
        if (!mShapes.empty())
        {
            mShapes.pop_back();
        }
    }

    std::unique_ptr<Shape> releaseLastShape()
    {
        if (mShapes.empty()) return nullptr;
        auto p = std::move(mShapes.back());
        mShapes.pop_back();
        return p;
    }

    void clearShapes()
    {
        mShapes.clear();
    }

    std::vector<std::unique_ptr<Shape>> releaseAllShapes()
    {
        std::vector<std::unique_ptr<Shape>> temp;
        temp.reserve(mShapes.size());
        for (auto& s : mShapes)
        {
            temp.push_back(std::move(s));
        }
        mShapes.clear();
        return temp;
    }

    void restoreShapes(std::vector<std::unique_ptr<Shape>>& shapes)
    {
        mShapes.clear();
        for (auto& s : shapes)
        {
            mShapes.push_back(std::move(s));
        }
    }

    size_t getNumberOfShapes() const
    {
        return mShapes.size();
    }

    Vector2 getPosition() const
    {
        return {mField.x, mField.y};
    }

    Vector2 getSize() const
    {
        return {mField.width, mField.height};
    }

    Shape& getShape(int index)
    {
        return *mShapes.at(index);
    }

    void draw() const
    {
        for (auto& p : mShapes)
        {
            p->draw();
        }
        DrawRectangleRec(mField, {0, 100, 200, 40});
    }
};

class Command
{
public:
    virtual void execute() = 0;
    virtual void unexecute() = 0;
    virtual ~Command() = default;
};

class NoCommand : public Command
{
public:
    void execute() override {}
    void unexecute() override {}
};

class RotateCommand : public Command
{
private:
    ShapePool& mPool;
    size_t mIndex;
    float mAngle;

public:
    RotateCommand(ShapePool& pool, size_t index, float angle) : mPool(pool), mIndex(index), mAngle(angle) {}

    void execute() override
    {
        if (mIndex < mPool.getNumberOfShapes())
        {
            mPool.getShape(mIndex).rotate(mAngle);
        }
    }

    void unexecute() override
    {
        if (mIndex < mPool.getNumberOfShapes())
        {
            mPool.getShape(mIndex).rotate(-mAngle);
        }
    }
};

class RandomColorCommand : public Command
{
private:
    ShapePool& mPool;
    size_t mIndex;
    Color mOldColor;
    Color mNewColor;
    bool mIsFirstRun {true};

public:
    RandomColorCommand(ShapePool& pool, size_t index) : mPool(pool), mIndex(index) {}

    void execute() override
    {
        if (mIndex < mPool.getNumberOfShapes())
        {
            mOldColor = mPool.getShape(mIndex).getColor();
            if (mIsFirstRun)
            {
                mNewColor = getRandomColor();
                mIsFirstRun = false;
            }
            mPool.getShape(mIndex).setColor(mNewColor);
        }
    }

    void unexecute() override
    {
        if (mIndex < mPool.getNumberOfShapes())
        {
            mPool.getShape(mIndex).setColor(mOldColor);
        }
    }
};

class RandomAllColorsCommand : public Command
{
private:
    ShapePool& mPool;
    std::vector<Color> mOldColors;
    std::vector<Color> mNewColors;
    bool mIsFirstRun {true};

public:
    RandomAllColorsCommand(ShapePool& pool) : mPool(pool) {}

    void execute() override
    {
        size_t count = mPool.getNumberOfShapes();
        mOldColors.resize(count);
        if (mIsFirstRun)
        {
            mNewColors.resize(count);
            for (size_t i = 0; i < count; ++i)
            {
                mNewColors[i] = getRandomColor();
            }
            mIsFirstRun = false;
        }
        for (size_t i = 0; i < count; ++i)
        {
            mOldColors[i] = mPool.getShape(i).getColor();
            mPool.getShape(i).setColor(mNewColors[i]);
        }
    }

    void unexecute() override
    {
        size_t count = std::min(mOldColors.size(), mPool.getNumberOfShapes());
        for (size_t i = 0; i < count; ++i)
        {
            mPool.getShape(i).setColor(mOldColors[i]);
        }
    }
};

class RandomAllPositionsCommand : public Command
{
private:
    ShapePool& mPool;
    std::vector<Vector2> mOldPositions;
    std::vector<Vector2> mNewPositions;
    bool mIsFirstRun {true};

public:
    RandomAllPositionsCommand(ShapePool& pool) : mPool(pool) {}

    void execute() override
    {
        size_t count = mPool.getNumberOfShapes();
        mOldPositions.resize(count);
        if (mIsFirstRun)
        {
            mNewPositions.resize(count);
            for (size_t i = 0; i < count; ++i)
            {
                Shape& shape = mPool.getShape(i);
                Vector2 size = shape.getSize();
                Vector2 min = {size.x / 2, size.y / 2};
                Vector2 max = {mPool.getSize().x - size.x / 2, mPool.getSize().y - size.y / 2};
                mNewPositions[i] = {mPool.getPosition().x + getRandomFloat(min.x, max.x), mPool.getPosition().y + getRandomFloat(min.y, max.y)};
            }
            mIsFirstRun = false;
        }
        for (size_t i = 0; i < count; ++i)
        {
            mOldPositions[i] = mPool.getShape(i).getPosition();
            mPool.getShape(i).setPosition(mNewPositions[i]);
        }
    }

    void unexecute() override
    {
        size_t count = std::min(mOldPositions.size(), mPool.getNumberOfShapes());
        for (size_t i = 0; i < count; ++i)
        {
            mPool.getShape(i).setPosition(mOldPositions[i]);
        }
    }
};

class AddNewRandomShape : public Command
{
private:
    ShapePool& mPool;
    float mMinSize;
    float mMaxSize;
    std::unique_ptr<Shape> mShape {nullptr};

public:
    AddNewRandomShape(ShapePool& pool, float minSize, float maxSize) : mPool(pool), mMinSize(minSize), mMaxSize(maxSize) {}

    void execute() override
    {
        if (!mShape)
        {
            int r = getRandomInt(0, 2);
            if (r == 0)
            {
                mShape = std::make_unique<RectangleShape>(Vector2{getRandomFloat(mMinSize, mMaxSize), getRandomFloat(mMinSize, mMaxSize)});
            }
            else if (r == 1)
            {
                mShape = std::make_unique<CircleShape>(getRandomFloat(mMinSize / 2, mMaxSize / 2));
            }
            else
            {
                mShape = std::make_unique<CircleShape>(getRandomFloat(mMinSize / 2, mMaxSize / 2), 3);
            }
            mShape->setColor(getRandomColor());
            mShape->rotate(getRandomFloat(0, 360));
            Vector2 size = mShape->getSize();
            Vector2 pos = {mPool.getPosition().x + getRandomFloat(0, mPool.getSize().x - size.x), mPool.getPosition().y + getRandomFloat(0, mPool.getSize().y - size.y)};
            mShape->setPosition(pos);
        }
        mPool.addShape(std::move(mShape));
    }

    void unexecute() override
    {
        mShape = mPool.releaseLastShape();
    }
};

class ClearAllShapesCommand : public Command
{
private:
    ShapePool& mPool;
    std::vector<std::unique_ptr<Shape>> mOldShapes;

public:
    ClearAllShapesCommand(ShapePool& pool) : mPool(pool) {}

    void execute() override
    {
        mOldShapes = mPool.releaseAllShapes();
    }

    void unexecute() override
    {
        mPool.restoreShapes(mOldShapes);
    }
};

class MultiCommand : public Command
{
private:
    std::vector<std::unique_ptr<Command>> mCommands;

public:
    MultiCommand(std::vector<std::unique_ptr<Command>> commands) : mCommands(std::move(commands)) {}

    void execute() override
    {
        for (auto& cmd : mCommands)
        {
            cmd->execute();
        }
    }

    void unexecute() override
    {
        for (auto it = mCommands.rbegin(); it != mCommands.rend(); ++it)
        {
            (*it)->unexecute();
        }
    }
};

class ControlPanel
{
private:
    std::vector<std::unique_ptr<Button>> mButtons;
    std::vector<std::unique_ptr<Command>> mCommands;
    std::vector<Command*> mHistory;

public:
    void addButton(std::unique_ptr<Button> p)
    {
        mButtons.push_back(std::move(p));
        mCommands.push_back(std::make_unique<NoCommand>());
    }

    void addButton(std::unique_ptr<Button> pb, std::unique_ptr<Command> pc)
    {
        mButtons.push_back(std::move(pb));
        mCommands.push_back(std::move(pc));
    }

    void draw()
    {
        for (auto& p : mButtons)
        {
            p->draw();
        }
    }

    void undoLastCommand()
    {
        if (!mHistory.empty())
        {
            Command* cmd = mHistory.back();
            cmd->unexecute();
            mHistory.pop_back();
        }
    }

    void update()
    {
        for (size_t i = 0; i < mButtons.size(); ++i)
        {
            if (mButtons[i]->handle())
            {
                if (i == mButtons.size() - 1)
                {
                    undoLastCommand();
                }
                else
                {
                    mCommands[i]->execute();
                    mHistory.push_back(mCommands[i].get());
                }
            }
        }
    }
};

int main()
{
    InitWindow(800, 800, "Shapes and Command");
    SetTargetFPS(60);

    ShapePool pool({300, 50}, {450, 700});
    
    auto r1 = std::make_unique<RectangleShape>(Vector2{getRandomFloat(50, 150), getRandomFloat(50, 150)});
    r1->setPosition({pool.getPosition().x + getRandomFloat(50, 300), pool.getPosition().y + getRandomFloat(50, 500)});
    r1->setColor(getRandomColor());
    pool.addShape(std::move(r1));

    auto r2 = std::make_unique<RectangleShape>(Vector2{getRandomFloat(50, 150), getRandomFloat(50, 150)});
    r2->setPosition({pool.getPosition().x + getRandomFloat(50, 300), pool.getPosition().y + getRandomFloat(50, 500)});
    r2->setColor(getRandomColor());
    pool.addShape(std::move(r2));

    auto c1 = std::make_unique<CircleShape>(getRandomFloat(30, 100), 3);
    c1->setPosition({pool.getPosition().x + getRandomFloat(50, 300), pool.getPosition().y + getRandomFloat(50, 500)});
    c1->setColor(getRandomColor());
    pool.addShape(std::move(c1));

    auto c2 = std::make_unique<CircleShape>(getRandomFloat(30, 100));
    c2->setPosition({pool.getPosition().x + getRandomFloat(50, 300), pool.getPosition().y + getRandomFloat(50, 500)});
    c2->setColor(getRandomColor());
    pool.addShape(std::move(c2));

    auto c3 = std::make_unique<CircleShape>(getRandomFloat(30, 100));
    c3->setPosition({pool.getPosition().x + getRandomFloat(50, 300), pool.getPosition().y + getRandomFloat(50, 500)});
    c3->setColor(getRandomColor());
    pool.addShape(std::move(c3));

    ControlPanel control;
    control.addButton(std::make_unique<Button>(Rectangle{40, 80, 240, 40}, "Rotate First"), std::make_unique<RotateCommand>(pool, 0, 30));
    control.addButton(std::make_unique<Button>(Rectangle{40, 140, 240, 40}, "Rotate Second"), std::make_unique<RotateCommand>(pool, 1, 45));
    control.addButton(std::make_unique<Button>(Rectangle{40, 200, 240, 40}, "One Random Color"), std::make_unique<RandomColorCommand>(pool, 3));
    control.addButton(std::make_unique<Button>(Rectangle{40, 260, 240, 40}, "All Random Positions"), std::make_unique<RandomAllPositionsCommand>(pool));
    control.addButton(std::make_unique<Button>(Rectangle{40, 320, 240, 40}, "New Random Shape"), std::make_unique<AddNewRandomShape>(pool, 10, 150));
    control.addButton(std::make_unique<Button>(Rectangle{40, 380, 240, 40}, "All Random Colors"), std::make_unique<RandomAllColorsCommand>(pool));
    control.addButton(std::make_unique<Button>(Rectangle{40, 440, 240, 40}, "Clear All Shapes"), std::make_unique<ClearAllShapesCommand>(pool));
    
    std::vector<std::unique_ptr<Command>> subCommands;
    subCommands.push_back(std::make_unique<RandomAllColorsCommand>(pool));
    subCommands.push_back(std::make_unique<RandomAllPositionsCommand>(pool));
    control.addButton(std::make_unique<Button>(Rectangle{40, 500, 240, 40}, "Colors & Positions"), std::make_unique<MultiCommand>(std::move(subCommands)));
    
    control.addButton(std::make_unique<Button>(Rectangle{40, 700, 240, 40}, "Undo"));

    while (!WindowShouldClose())
    {
        control.update();

        BeginDrawing();
        ClearBackground(BLACK);

        pool.draw();
        control.draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}