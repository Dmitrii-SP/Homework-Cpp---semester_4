#include <iostream>
#include <vector>

class Individual
{
public:
    virtual void speak() const = 0;
    virtual int getId() const = 0;
    virtual ~Individual() = default;
};

class Alice : public Individual
{
public:
    void speak() const override
    {
        std::cout << "Hi, I am Alice" << std::endl;
    }
    int getId() const override { return 0; }
};

class Bob : public Individual
{
public:
    void speak() const override
    {
        std::cout << "Hi, I am Bob" << std::endl;
    }
    int getId() const override { return 1; }
};

class Casper : public Individual
{
public:
    void speak() const override
    {
        std::cout << "Hi, I am Casper" << std::endl;
    }
    int getId() const override { return 2; }
};

int identification(Individual* p)
{
    if (!p) return -1;
    return p->getId();
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