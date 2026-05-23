#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <optional>
#include <algorithm>
#include <cctype>
#include <atomic>

template <typename T>
class SafeQueue {
private:
    std::queue<T> q;
    mutable std::mutex mtx;
public:
    void push(T value) {
        std::lock_guard<std::mutex> lock(mtx);
        q.push(std::move(value));
    }
    std::optional<T> pop() {
        std::lock_guard<std::mutex> lock(mtx);
        if (q.empty()) return std::nullopt;
        T value = std::move(q.front());
        q.pop();
        return value;
    }
    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx);
        return q.empty();
    }
};

SafeQueue<std::string> shared_queue;
std::atomic<bool> producer_done(false);

void producer(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        producer_done = true;
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            shared_queue.push(line);
        }
    }
    producer_done = true;
}

void consumer() {
    while (true) {
        auto item = shared_queue.pop();
        if (item) {
            std::string line = *item;
            std::transform(line.begin(), line.end(), line.begin(), [](unsigned char c) {
                return std::toupper(c);
            });
            std::cout << line << std::endl;
        } else if (producer_done) {
            break;
        } else {
            std::this_thread::yield();
        }
    }
}

int main() {
    std::thread t1(producer, "invisible_man.txt");
    std::thread t2(consumer);
    t1.join();
    t2.join();
    return 0;
}