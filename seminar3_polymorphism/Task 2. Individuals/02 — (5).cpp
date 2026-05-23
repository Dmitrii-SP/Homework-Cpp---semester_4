#include <iostream>
#include <vector>
#include <memory> 

struct Individual {
    virtual void speak() = 0;
    virtual ~Individual() = default; 
};

struct Alice : public Individual {
    void speak() override { 
        std::cout << "Alice\n"; 
    }
};

struct Bob : public Individual {
    void speak() override { 
        std::cout << "Bob\n"; 
    }
};

struct Casper : public Individual {
    void speak() override { 
        std::cout << "Casper\n"; 
    }
};

std::vector<std::unique_ptr<Individual>> createIndividualsSmart() {
    std::vector<std::unique_ptr<Individual>> individuals;

    for (int i = 0; i < 3; ++i) individuals.push_back(std::make_unique<Alice>());
    for (int i = 0; i < 2; ++i) individuals.push_back(std::make_unique<Bob>());
    for (int i = 0; i < 4; ++i) individuals.push_back(std::make_unique<Casper>());

    return individuals;
}

void letThemSpeakSmart(const std::vector<std::unique_ptr<Individual>>& individuals) {
    for (const auto& p : individuals) {
        if (p) {
            p->speak(); 
        }
    }
}

int main() {
    std::vector<std::unique_ptr<Individual>> individuals = createIndividualsSmart();

    letThemSpeakSmart(individuals);

    return 0;
}