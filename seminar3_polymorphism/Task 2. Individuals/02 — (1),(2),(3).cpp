#include <iostream>
#include <vector>

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

std::vector<Individual*> createIndividuals()
{
    std::vector<Individual*> v;

    for (int i = 0; i < 3; ++i) v.push_back(new Alice());
    for (int i = 0; i < 2; ++i) v.push_back(new Bob());
    for (int i = 0; i < 4; ++i) v.push_back(new Casper());

    return v;
}

void letThemSpeak(const std::vector<Individual*>& v)
{
    for (Individual* p : v)
    {
        if (p)
        {
            p->speak();
        }
    }
}

void deleteIndividuals(std::vector<Individual*>& v)
{
    for (Individual* p : v)
    {
        delete p;
    }
    v.clear(); 
}

int main()
{
    std::vector<Individual*> v = createIndividuals();

    letThemSpeak(v);

    deleteIndividuals(v);
    return 0;
}