#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <future>
#include <chrono>
#include <thread>

uint64_t getMax(const std::vector<uint64_t>& v) {
    uint64_t result = v.at(0);
    for (auto el : v) {
        if (el > result)
            result = el;
    }
    return result;
}

uint64_t getMax(int n, const std::vector<uint64_t>& v) {
    size_t size = v.size();
    size_t chunk_size = size / n;
    std::vector<std::future<uint64_t>> futures;

    for (int i = 0; i < n; ++i) {
        size_t start = i * chunk_size;
        size_t end = (i == n - 1) ? size : (i + 1) * chunk_size;

        futures.push_back(std::async(std::launch::async, [&v, start, end]() {
            uint64_t local_max = v[start];
            for (size_t j = start + 1; j < end; ++j) {
                if (v[j] > local_max)
                    local_max = v[j];
            }
            return local_max;
        }));
    }

    uint64_t global_max = futures[0].get();
    for (size_t i = 1; i < futures.size(); ++i) {
        uint64_t val = futures[i].get();
        if (val > global_max)
            global_max = val;
    }
    return global_max;
}

int main() {
    std::vector<uint64_t> numbers(5e8);
    numbers[0] = 123456789;
    for (size_t i = 1; i < numbers.size(); ++i) {
        numbers[i] = numbers[i - 1] * i + 1;
    }

    auto start_single = std::chrono::high_resolution_clock::now();
    uint64_t res_single = getMax(numbers);
    auto end_single = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff_single = end_single - start_single;

    int n_threads = std::thread::hardware_concurrency();
    
    auto start_async = std::chrono::high_resolution_clock::now();
    uint64_t res_async = getMax(n_threads, numbers);
    auto end_async = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff_async = end_async - start_async;

    std::cout << "Single-threaded: " << diff_single.count() << "s, Max: " << res_single << std::endl;
    std::cout << "Multi-threaded (" << n_threads << "): " << diff_async.count() << "s, Max: " << res_async << std::endl;

    return 0;
}