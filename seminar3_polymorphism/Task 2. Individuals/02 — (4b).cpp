#include <iostream>

class Individual
{
public:
    virtual void speak() const = 0;
    virtual ~Individual() = default;
};

class Alice : public Individual
{
public:
    void speak() const override
    {
        std::cout << "Hi, I am Alice" << std::endl;
    }
};

class Bob : public Individual
{
public:
    void speak() const override
    {
        std::cout << "Hi, I am Bob" << std::endl;
    }
};

class Casper : public Individual
{
public:
    void speak() const override
    {
        std::cout << "Hi, I am Casper" << std::endl;
    }
};

int identification(Individual* p)
{
    if (!p) return -1;

    if (dynamic_cast<Alice*>(p))  return 0;
    if (dynamic_cast<Bob*>(p))    return 1;
    if (dynamic_cast<Casper*>(p)) return 2;

    return -1;
}

int main()
{
    Alice a;
    Bob b;
    Casper c;

    std::cout << "Alice ID: " << identification(&a) << std::endl;
    std::cout << "Bob ID: "   << identification(&b) << std::endl;
    std::cout << "Casper ID: "<< identification(&c) << std::endl;

    return 0;
}