#include <iostream>
#include <queue>
#include <mutex>
#include <optional>

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
        if (q.empty()) {
            return std::nullopt;
        }
        T value = std::move(q.front());
        q.pop();
        return value;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx);
        return q.empty();
    }
};

int main() {
    SafeQueue<int> sq;

    sq.push(10);
    sq.push(20);

    auto val1 = sq.pop();
    if (val1) std::cout << "Popped: " << *val1 << std::endl;

    auto val2 = sq.pop();
    if (val2) std::cout << "Popped: " << *val2 << std::endl;

    auto val3 = sq.pop();
    if (!val3) std::cout << "Queue is empty" << std::endl;

    return 0;
}

// Popped: 10
// Popped: 20
// Queue is empty