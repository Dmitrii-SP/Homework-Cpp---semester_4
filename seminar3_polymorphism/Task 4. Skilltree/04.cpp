#include "raylib.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <stdexcept>

class TextureManager
{
private:
    std::unordered_map<std::string, Texture2D> mTextures;

public:
    ~TextureManager()
    {
        for (auto& [_, tex] : mTextures)
        {
            UnloadTexture(tex);
        }
    }

    Texture2D& get(const std::string& path)
    {
        if (mTextures.count(path) == 0)
        {
            Texture2D tex = LoadTexture(path.c_str());
            if (tex.id == 0)
            {
                throw std::runtime_error("Failed to load texture: " + path);
            }
            mTextures[path] = tex;
        }
        return mTextures[path];
    }
};

class Node
{
public:
    enum class State
    {
        Blocked,
        Unblocked,
        Activated
    };

protected:
    Vector2 mPosition;
    State mState { State::Blocked };
    std::vector<std::unique_ptr<Node>> mChildren;

public:
    Node(Vector2 pos)
        : mPosition(pos)
    {
    }

    void addChild(std::unique_ptr<Node> child)
    {
        mChildren.push_back(std::move(child));
    }

    void unblock()
    {
        mState = State::Unblocked;
    }

    void block()
    {
        mState = State::Blocked;
        for (auto& c : mChildren)
        {
            c->block();
        }
    }

    const Vector2& getPosition() const
    {
        return mPosition;
    }

    Node* findClicked(Vector2 p)
    {
        if (contains(p))
        {
            return this;
        }

        for (auto& c : mChildren)
        {
            if (auto* res = c->findClicked(p))
            {
                return res;
            }
        }

        return nullptr;
    }    

    virtual bool contains(Vector2 p) const = 0;
    virtual void onClick() = 0;
    virtual void draw() const = 0;
    
    virtual ~Node() = default;
};

class HitNode : public Node
{
protected:
    float mRadius { 24.f };
    Texture2D* mTexture { nullptr };

public:
    HitNode(Vector2 pos, const std::string& icon, TextureManager& tm)
        : Node(pos)
    {
        mTexture = &tm.get(icon);
    }

    bool contains(Vector2 p) const override
    {
        float dx = p.x - mPosition.x;
        float dy = p.y - mPosition.y;
        return dx * dx + dy * dy <= mRadius * mRadius;
    }

    void onClick() override
    {
        if (mState == State::Blocked)
        {
            return;
        }

        if (mState == State::Unblocked)
        {
            mState = State::Activated;
            for (auto& c : mChildren)
            {
                c->unblock();
            }
        }
        else if (mState == State::Activated)
        {
            mState = State::Unblocked;
            for (auto& c : mChildren)
            {
                c->block();
            }
        }
    }

    Color getColor() const
    {
        switch (mState)
        {
            case State::Blocked:   return {40, 40, 40, 255};
            case State::Unblocked: return {80, 80, 40, 255};
            case State::Activated: return {160, 160, 40, 255};
        }
        return WHITE;
    }

    void draw() const override
    {
        for (const auto& c : mChildren)
        {
            DrawLineEx(mPosition, c->getPosition(), 2.0f, getColor());
            c->draw();
        }

        DrawCircleV(mPosition, mRadius, getColor());

        if (mTexture)
        {
            DrawTextureEx(*mTexture,
                          { mPosition.x - mRadius, mPosition.y - mRadius },
                          0.0f,
                          (mRadius * 2) / mTexture->width,
                          WHITE);
        }
    }
};

class AccumulativeNode : public Node
{
protected:
    float mSize { 48.f };    
    int mCurrentLevel { 0 }; 
    int mMaxLevel { 1 };    
    Texture2D* mTexture { nullptr };

public:
    AccumulativeNode(Vector2 pos, const std::string& icon, TextureManager& tm, int maxLevel)
        : Node(pos), mMaxLevel(maxLevel)
    {
        mTexture = &tm.get(icon);
    }

    bool contains(Vector2 p) const override
    {
        float halfSize = mSize / 2.0f;
        return (p.x >= mPosition.x - halfSize && p.x <= mPosition.x + halfSize &&
                p.y >= mPosition.y - halfSize && p.y <= mPosition.y + halfSize);
    }

    void onClick() override
    {
        if (mState == State::Blocked)
        {
            return;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (mState == State::Unblocked)
            {
                mState = State::Activated;
                mCurrentLevel = 1;
                for (auto& c : mChildren)
                {
                    c->unblock();
                }
            }
            else if (mState == State::Activated)
            {
                if (mCurrentLevel < mMaxLevel)
                {
                    mCurrentLevel++;
                }
            }
        }

        else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        {
            if (mState == State::Activated)
            {
                mCurrentLevel--;

                if (mCurrentLevel == 0)
                {
                    mState = State::Unblocked;
                    for (auto& c : mChildren)
                    {
                        c->block(); 
                    }
                }
            }
        }
    }

    Color getColor() const
    {
        switch (mState)
        {
            case State::Blocked:   return {40, 40, 40, 255};
            case State::Unblocked: return {80, 80, 40, 255};
            case State::Activated: return {160, 160, 40, 255};
        }
        return WHITE;
    }

    void draw() const override
    {
        for (const auto& c : mChildren)
        {
            DrawLineEx(mPosition, c->getPosition(), 2.0f, getColor());
            c->draw();
        }

        float halfSize = mSize / 2.0f;
        Vector2 topLeft = { mPosition.x - halfSize, mPosition.y - halfSize };

        DrawRectangleV(topLeft, {mSize, mSize}, getColor());

        if (mTexture)
        {
            DrawTextureEx(*mTexture,
                          topLeft,
                          0.0f,
                          mSize / mTexture->width,
                          WHITE);
        }

        std::string levelText = std::to_string(mCurrentLevel) + "/" + std::to_string(mMaxLevel);
        int fontSize = 16;
        int textWidth = MeasureText(levelText.c_str(), fontSize);
        
        float textX = mPosition.x - textWidth / 2.0f;
        float textY = mPosition.y + halfSize + 4.0f;

        DrawText(levelText.c_str(), (int)textX, (int)textY, fontSize, WHITE);
    }
};

class SelectorNode : public Node
{
protected:
    float mSize { 48.f };
    std::vector<Texture2D*> mModTextures; 
    int mCurrentIndex { -1 };            

public:

    SelectorNode(Vector2 pos, const std::vector<std::string>& iconPaths, TextureManager& tm)
        : Node(pos)
    {
        for (const auto& path : iconPaths)
        {
            mModTextures.push_back(&tm.get(path));
        }
    }

    bool contains(Vector2 p) const override
    {
        float halfSize = mSize / 2.0f;
        return (p.x >= mPosition.x - halfSize && p.x <= mPosition.x + halfSize &&
                p.y >= mPosition.y - halfSize && p.y <= mPosition.y + halfSize);
    }

    void onClick() override
    {
        if (mState == State::Blocked || mModTextures.empty())
        {
            return;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (mState == State::Unblocked)
            {
                mState = State::Activated;
                mCurrentIndex = 0; 
                for (auto& c : mChildren)
                {
                    c->unblock();
                }
            }
            else if (mState == State::Activated)
            {
                mCurrentIndex = (mCurrentIndex + 1) % mModTextures.size();
            }
        }
        else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        {
            if (mState == State::Activated)
            {
                mState = State::Unblocked;
                mCurrentIndex = -1; 
                for (auto& c : mChildren)
                {
                    c->block(); 
                }
            }
        }
    }

    Color getColor() const
    {
        switch (mState)
        {
            case State::Blocked:   return {40, 40, 40, 255};
            case State::Unblocked: return {80, 80, 40, 255};
            case State::Activated: return {40, 160, 160, 255}; 
        }
        return WHITE;
    }

    void draw() const override
    {
        for (const auto& c : mChildren)
        {
            DrawLineEx(mPosition, c->getPosition(), 2.0f, getColor());
            c->draw();
        }

        float halfSize = mSize / 2.0f;
        Vector2 topLeft = { mPosition.x - halfSize, mPosition.y - halfSize };

        DrawRectangleV(topLeft, {mSize, mSize}, getColor());
        DrawRectangleLinesEx({topLeft.x, topLeft.y, mSize, mSize}, 2.0f, BLACK);

        if (mState == State::Activated && mCurrentIndex >= 0 && mCurrentIndex < mModTextures.size())
        {
            Texture2D* currentTex = mModTextures[mCurrentIndex];
            if (currentTex)
            {
                DrawTextureEx(*currentTex,
                              topLeft,
                              0.0f,
                              mSize / currentTex->width,
                              WHITE);
            }
        }

        std::string label = (mCurrentIndex == -1) ? "Empty" : "Mode: " + std::to_string(mCurrentIndex + 1);
        int fontSize = 14;
        int textWidth = MeasureText(label.c_str(), fontSize);
        DrawText(label.c_str(), (int)(mPosition.x - textWidth / 2.0f), (int)(mPosition.y + halfSize + 4.0f), fontSize, WHITE);
    }
};

class ModifierNode : public Node
{
public:
    enum class Type { Fire, Ice, Shadow };
    
protected:
    Type mCurrentType { Type::Fire };
    float mSize { 50.f };
    Texture2D* mTexture { nullptr };

public:
    ModifierNode(Vector2 pos, TextureManager& tm) : Node(pos)
    {
        mTexture = &tm.get("icons/icon_rect_chain.png"); 
    }

    bool contains(Vector2 p) const override {
        return CheckCollisionPointRec(p, {mPosition.x-25, mPosition.y-25, 50, 50}); 
    }

    void onClick() override 
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            mCurrentType = static_cast<Type>((static_cast<int>(mCurrentType) + 1) % 3);
        }
    }

    Type getActiveModifier() const { return mCurrentType; }

    void draw() const override
    {
        Color c = (mCurrentType == Type::Fire) ? RED : (mCurrentType == Type::Ice ? BLUE : PURPLE);
        DrawRectangleV({mPosition.x-25, mPosition.y-25}, {50, 50}, c);
        
        for (auto& child : mChildren) {
            child->draw(); 
        }
    }
};

std::unique_ptr<Node> createTree(TextureManager& tm)
{
    auto root = std::make_unique<HitNode>(Vector2{400, 600}, "icons/icon_lightning.png", tm);

    auto a = std::make_unique<HitNode>(Vector2{200, 450}, "icons/icon_shuriken.png", tm);
    a->addChild(std::make_unique<HitNode>(Vector2{100, 300}, "icons/icon_shield.png", tm));
    a->addChild(std::make_unique<HitNode>(Vector2{200, 300}, "icons/icon_sword.png", tm));

    auto b = std::make_unique<AccumulativeNode>(Vector2{400, 450}, "icons/icon_fireball.png", tm, 5);
    auto deep = std::make_unique<AccumulativeNode>(Vector2{480, 300}, "icons/icon_bomb.png", tm, 3);
    deep->addChild(std::make_unique<HitNode>(Vector2{480, 150}, "icons/icon_spikes.png", tm));
    b->addChild(std::make_unique<HitNode>(Vector2{320, 300}, "icons/icon_hand.png", tm));
    b->addChild(std::make_unique<HitNode>(Vector2{400, 300}, "icons/icon_meteorite.png", tm));
    b->addChild(std::move(deep));

    std::vector<std::string> modifiers = {
        "icons/icon_rect_sword.png",
        "icons/icon_rect_freeze.png",
        "icons/icon_rect_chain.png"
    };

    auto c = std::make_unique<SelectorNode>(Vector2{600, 450}, modifiers, tm);
    
    c->addChild(std::make_unique<HitNode>(Vector2{700, 300}, "icons/icon_wind.png", tm));

    root->addChild(std::move(a));
    root->addChild(std::move(b));
    root->addChild(std::move(c));

    return root;
}

int main()
{
    InitWindow(800, 800, "Skill Tree (raylib)");
    SetTargetFPS(60);

    TextureManager tm;
    auto root = createTree(tm);
    root->unblock();

    while (!WindowShouldClose())
    {
        Vector2 mouse = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        {
            if (auto* node = root->findClicked(mouse))
            {
                node->onClick();
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        root->draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}