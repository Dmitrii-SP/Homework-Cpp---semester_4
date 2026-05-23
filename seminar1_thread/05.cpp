#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <random>

template <typename RandIt, typename Comparator>
void parallelSort(int n, RandIt start, RandIt finish, Comparator comp)
{
    auto dist = std::distance(start, finish);
    if (n <= 1 || dist < n)
    {
        std::sort(start, finish, comp);
        return;
    }

    std::vector<std::thread> threads;
    auto chunk_size = dist / n;

    for (int i = 0; i < n; ++i)
    {
        RandIt chunk_start = start + (i * chunk_size);
        RandIt chunk_finish = (i == n - 1) ? finish : (chunk_start + chunk_size);

        threads.emplace_back([chunk_start, chunk_finish, comp]() {
            std::sort(chunk_start, chunk_finish, comp);
        });
    }

    for (auto& t : threads)
    {
        t.join();
    }

    for (int size = chunk_size; size < dist; size += chunk_size)
    {
        RandIt merge_finish = (size + chunk_size >= dist) ? finish : (start + size + chunk_size);
        std::inplace_merge(start, start + size, merge_finish, comp);
    }
}

int main()
{
    const size_t ELEMENT_COUNT = 10'000'000;
    std::vector<int> v1(ELEMENT_COUNT);
    
    std::mt19937 gen(42);
    std::uniform_int_distribution<int> dis(1, 100'000'000);
    for (size_t i = 0; i < ELEMENT_COUNT; ++i)
    {
        v1[i] = dis(gen);
    }
    std::vector<int> v2 = v1;

    auto start_time = std::chrono::high_resolution_clock::now();
    std::sort(v1.begin(), v1.end(), [](int a, int b) { return a > b; });
    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << "Single-thread sort: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
              << " ms." << std::endl;

    int num_threads = 4;
    start_time = std::chrono::high_resolution_clock::now();
    parallelSort(num_threads, v2.begin(), v2.end(), [](int a, int b) { return a > b; });
    end_time = std::chrono::high_resolution_clock::now();
    std::cout << "Multi-thread sort (" << num_threads << " threads): "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
              << " ms." << std::endl;

    return 0;
}

// Single-thread sort: 7245 ms.
// Multi-thread sort (4 threads): 5866 ms.