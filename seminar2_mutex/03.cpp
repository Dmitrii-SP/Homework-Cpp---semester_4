#include <iostream>
#include <mutex>
#include <memory>
#include <optional>

template <typename T>
class SafeQueue {
private:
    struct Node {
        T data;
        std::unique_ptr<Node> next;
        Node(T val) : data(std::move(val)), next(nullptr) {}
    };

    std::unique_ptr<Node> head;
    Node* tail; 
    
    std::mutex head_mutex;
    std::mutex tail_mutex;

public:
    SafeQueue() : head(nullptr), tail(nullptr) {}

    void push(T value) {
        auto new_node = std::make_unique<Node>(std::move(value));
        Node* const new_tail = new_node.get();

        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        
        if (tail) {
            tail->next = std::move(new_node);
        } else {
            std::lock_guard<std::mutex> head_lock(head_mutex);
            head = std::move(new_node);
        }
        tail = new_tail;
    }

    std::optional<T> pop() {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        
        if (!head) {
            return std::nullopt;
        }

        T res = std::move(head->data);
        head = std::move(head->next);

        if (!head) {
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            tail = nullptr;
        }

        return res;
    }
};

int main() {
    SafeQueue<int> q;

    q.push(10);
    q.push(20);

    auto v1 = q.pop();
    if (v1) std::cout << "Popped: " << *v1 << std::endl;

    auto v2 = q.pop();
    if (v2) std::cout << "Popped: " << *v2 << std::endl;

    auto v3 = q.pop();
    if (!v3) std::cout << "Queue is empty" << std::endl;

    return 0;
}

// Popped: 10
// Popped: 20
// Queue is empty