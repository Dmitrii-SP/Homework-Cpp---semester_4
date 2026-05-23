#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <iterator>
#include <deque>
#include <chrono>
#include <cstdint>

template <typename RandIt>
RandIt getMax(RandIt start, RandIt finish)
{
    if (start == finish) return finish;
    RandIt maxIt = start;
    for (auto it = start; it != finish; ++it)
    {
        if (*it > *maxIt)
            maxIt = it;
    }
    return maxIt;
}

template <typename RandIt>
RandIt getMax(int n, RandIt start, RandIt finish)
{
    auto dist = std::distance(start, finish);
    if (n <= 1 || dist < n) return getMax(start, finish);

    std::vector<std::thread> threads;
    std::vector<RandIt> partial_results(n);
    auto chunk_size = dist / n;

    for (int i = 0; i < n; ++i)
    {
        RandIt chunk_start = start + (i * chunk_size);
        RandIt chunk_finish = (i == n - 1) ? finish : (chunk_start + chunk_size);

        threads.emplace_back([chunk_start, chunk_finish, &partial_results, i]() {
            partial_results[i] = getMax(chunk_start, chunk_finish);
        });
    }

    for (auto& t : threads)
    {
        t.join();
    }

    RandIt global_max_it = partial_results[0];
    for (int i = 1; i < n; ++i)
    {
        if (*partial_results[i] > *global_max_it)
        {
            global_max_it = partial_results[i];
        }
    }

    return global_max_it;
}

int main()
{
    std::cout << "Generating numbers!" << std::endl;
    std::deque<uint64_t> numbers;
    for (size_t i = 0; i < 1e7; ++i)
    {
        numbers.push_back(i * 2 + 1);
    }
    numbers[5'000'000] = 999'999'999;
    std::cout << "Numbers generated!" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    auto it_single = getMax(numbers.begin(), numbers.end());
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Single-thread maximum = " << *it_single << std::endl;
    std::cout << "Time single = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms." << std::endl;

    int num_threads = 4;
    start = std::chrono::high_resolution_clock::now();
    auto it_multi = getMax(num_threads, numbers.begin(), numbers.end());
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Multi-thread maximum = " << *it_multi << std::endl;
    std::cout << "Time multi (" << num_threads << " threads) = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms." << std::endl;

    return 0;
}

// Generating numbers!
// Numbers generated!
// Single-thread maximum = 999999999
// Time single = 242 ms.
// Multi-thread maximum = 999999999
// Time multi (4 threads) = 162 ms.