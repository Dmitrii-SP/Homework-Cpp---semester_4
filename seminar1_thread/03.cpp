#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cstdint>

uint64_t getMax(const std::vector<uint64_t>& v)
{
    uint64_t result = v.at(0);
    for (auto el : v)
    {
        if (el > result)
            result = el;
    }
    return result;
}

uint64_t getMax(int n, const std::vector<uint64_t>& v)
{
    if (v.empty()) return 0;
    if (n <= 1 || v.size() < static_cast<size_t>(n)) return getMax(v);

    std::vector<std::thread> threads;
    std::vector<uint64_t> partial_maxima(n);
    size_t chunk_size = v.size() / n;

    for (int i = 0; i < n; ++i)
    {
        size_t start_idx = i * chunk_size;
        size_t end_idx = (i == n - 1) ? v.size() : start_idx + chunk_size;

        threads.emplace_back([&v, start_idx, end_idx, &partial_maxima, i]() {
            uint64_t local_max = v[start_idx];
            for (size_t j = start_idx; j < end_idx; ++j)
            {
                if (v[j] > local_max)
                {
                    local_max = v[j];
                }
            }
            partial_maxima[i] = local_max;
        });
    }

    for (auto& t : threads)
    {
        t.join();
    }

    return getMax(partial_maxima);
}

int main()
{
    std::cout << "Generating numbers!" << std::endl;
    std::vector<uint64_t> numbers(5e7);
    numbers[0] = 123456789;
    for (size_t i = 1; i < numbers.size(); ++i)
    {
        numbers[i] = numbers[i - 1] * i + 1;
    }
    std::cout << "Numbers generated!" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    uint64_t m_single = getMax(numbers);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Single-thread maximum = " << m_single << std::endl;
    std::cout << "Time single = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms." << std::endl;

    int num_threads = 4;
    start = std::chrono::high_resolution_clock::now();
    uint64_t m_multi = getMax(num_threads, numbers);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Multi-thread maximum = " << m_multi << std::endl;
    std::cout << "Time multi (" << num_threads << " threads) = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms." << std::endl;

    return 0;
}

// Generating numbers!
// Numbers generated!
// Single-thread maximum = 18446743998432771393
// Time single = 873 ms.
// Multi-thread maximum = 18446743998432771393
// Time multi (4 threads) = 269 ms.