#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <future>
#include <algorithm>

uint64_t findSegmentMax(const std::vector<uint64_t>& src, size_t left, size_t right) {
    uint64_t local_peak = src[left];
    for (size_t idx = left + 1; idx < right; ++idx) {
        if (src[idx] > local_peak) {
            local_peak = src[idx];
        }
    }
    return local_peak;
}

uint64_t getMax(const std::vector<uint64_t>& values) {
    if (values.empty()) return 0;
    
    uint64_t absolute_max = values[0];
    for (const auto& element : values) {
        if (element > absolute_max) {
            absolute_max = element;
        }
    }
    return absolute_max;
}

uint64_t getMax(int requested_threads, const std::vector<uint64_t>& values) {
    if (values.empty()) return 0;

    size_t worker_count = std::max(1, requested_threads);
    if (worker_count > values.size()) {
        worker_count = values.size();
    }

    std::vector<std::thread> active_threads;
    std::vector<std::future<uint64_t>> futures_list;
    
    active_threads.reserve(worker_count);
    futures_list.reserve(worker_count);

    const size_t chunk_size = values.size() / worker_count;

    for (size_t i = 0; i < worker_count; ++i) {
        size_t start_pos = i * chunk_size;
        size_t end_pos = (i == worker_count - 1) ? values.size() : (i + 1) * chunk_size;

        std::packaged_task<uint64_t()> current_task([&values, start_pos, end_pos]() {
            return findSegmentMax(values, start_pos, end_pos);
        });

        futures_list.push_back(current_task.get_future());
        active_threads.emplace_back(std::move(current_task));
    }

    for (auto& worker : active_threads) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    uint64_t global_max = futures_list[0].get();
    for (size_t i = 1; i < futures_list.size(); ++i) {
        uint64_t local_res = futures_list[i].get();
        if (local_res > global_max) {
            global_max = local_res;
        }
    }

    return global_max;
}

int main() {
    int execution_threads = 0;
    if (!(std::cin >> execution_threads)) return 1;

    std::cout << "Generating numbers!\n";
    std::vector<uint64_t> dataset(5e8);
    dataset[0] = 123456789;
    for (size_t i = 1; i < dataset.size(); ++i) {
        dataset[i] = dataset[i - 1] * i + 1;
    }
    std::cout << "Numbers generated!\n";

    auto single_start = std::chrono::steady_clock::now();
    uint64_t max_single = getMax(dataset);
    auto single_end = std::chrono::steady_clock::now();

    std::cout << "One thread maximum = " << max_single << "\n";
    std::cout << "One thread time = "
              << std::chrono::duration_cast<std::chrono::milliseconds>(single_end - single_start).count()
              << " milliseconds.\n";

    auto multi_start = std::chrono::steady_clock::now();
    uint64_t max_multi = getMax(execution_threads, dataset);
    auto multi_end = std::chrono::steady_clock::now();

    std::cout << "Packaged task maximum = " << max_multi << "\n";
    std::cout << "Packaged task time = "
              << std::chrono::duration_cast<std::chrono::milliseconds>(multi_end - multi_start).count()
              << " milliseconds.\n";
              
    return 0;
}