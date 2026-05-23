#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

void threadTask(int i)
{
    std::cout << "Thread #" << i << " started." << std::endl;
    
    std::this_thread::sleep_for(std::chrono::seconds(i));
    
    std::cout << "Thread #" << i << " finished." << std::endl;
}

int main()
{
    int n = 0;
    std::cout << "Enter the number of threads: ";
    if (!(std::cin >> n) || n <= 0)
    {
        std::cerr << "Invalid input. Please enter a positive integer." << std::endl;
        return 1;
    }

    std::vector<std::thread> threads;
    threads.reserve(n);

    for (int i = 1; i <= n; ++i)
    {
        threads.emplace_back(threadTask, i);
    }

    for (auto& th : threads)
    {
        if (th.joinable())
        {
            th.join();
        }
    }

    std::cout << "Main thread finished. All sub-threads have completed." << std::endl;
    return 0;
}