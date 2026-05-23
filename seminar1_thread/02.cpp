#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

void createThread(int i, int maxThreads)
{
    std::cout << "Thread #" << i << " started." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::thread nextThread;
    if (i < maxThreads)
    {
        nextThread = std::thread(createThread, i + 1, maxThreads);
    }

    if (nextThread.joinable())
    {
        nextThread.join();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::cout << "Thread #" << i << " finished." << std::endl;
}

int main()
{
    int n = 0;
    if (!(std::cin >> n) || n <= 0)
    {
        return 1;
    }

    std::thread firstThread(createThread, 1, n);
    if (firstThread.joinable())
    {
        firstThread.join();
    }

    return 0;
}