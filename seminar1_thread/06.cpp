#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

using namespace std::string_literals;

template <typename F, typename... Args>
void iterate(int n, F func, Args... args)
{
    if (n <= 0) return;

    std::vector<std::thread> threads;
    threads.reserve(n);

    for (int i = 0; i < n; ++i)
    {
        threads.emplace_back(func, args...);
    }

    for (auto& t : threads)
    {
        t.join();
    }
}

void func(const std::string& a, int b)
{
    std::cout << a << " " << b << std::endl;
}

int main()
{
    iterate(5, func, "Hello"s, 12345);
    return 0;
}