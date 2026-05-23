#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <cassert>
#include <algorithm>

template <typename T>
struct Node {
    T data;
    std::atomic<Node*> next;
    Node(T val) : data(val), next(nullptr) {}
};

template <typename T>
class LockFreeQueueDeferred {
private:
    std::atomic<Node<T>*> head;
    std::atomic<Node<T>*> tail;
    std::atomic<Node<T>*> retired_list;
    std::atomic<int> active_threads;

    void cleanup() {
        Node<T>* current = retired_list.exchange(nullptr);
        while (current) {
            Node<T>* next = current->next.load();
            delete current;
            current = next;
        }
    }

public:
    LockFreeQueueDeferred() {
        Node<T>* dummy = new Node<T>(T());
        head.store(dummy);
        tail.store(dummy);
        retired_list.store(nullptr);
        active_threads.store(0);
    }

    ~LockFreeQueueDeferred() {
        while (Node<T>* n = head.load()) {
            head.store(n->next.load());
            delete n;
        }
        cleanup();
    }

    void push(T val) {
        Node<T>* new_node = new Node<T>(val);
        Node<T>* last;
        Node<T>* next;
        while (true) {
            last = tail.load();
            next = last->next.load();
            if (last == tail.load()) {
                if (next == nullptr) {
                    if (last->next.compare_exchange_weak(next, new_node)) {
                        break;
                    }
                } else {
                    tail.compare_exchange_weak(last, next);
                }
            }
        }
        tail.compare_exchange_weak(last, new_node);
    }

    bool pop(T& result) {
        active_threads.fetch_add(1);
        Node<T>* first;
        Node<T>* last;
        Node<T>* next;
        bool success = false;

        while (true) {
            first = head.load();
            last = tail.load();
            next = first->next.load();

            if (first == head.load()) {
                if (first == last) {
                    if (next == nullptr) {
                        success = false;
                        break;
                    }
                    tail.compare_exchange_weak(last, next);
                } else {
                    result = next->data;
                    if (head.compare_exchange_weak(first, next)) {
                        Node<T>* old_retired = retired_list.load();
                        do {
                            first->next.store(old_retired);
                        } while (!retired_list.compare_exchange_weak(old_retired, first));
                        success = true;
                        break;
                    }
                }
            }
        }

        if (active_threads.fetch_sub(1) == 1) {
            cleanup();
        }
        return success;
    }
};

template <typename T>
class LockFreeQueueHP {
private:
    struct HazardPointer {
        std::atomic<Node<T>*> ptr;
        std::atomic<bool> active;
    };

    std::atomic<Node<T>*> head;
    std::atomic<Node<T>*> tail;
    static const int MAX_THREADS = 128;
    HazardPointer hp[MAX_THREADS];

    void retire_node(Node<T>* node) {
        delete node;
    }

public:
    LockFreeQueueHP() {
        Node<T>* dummy = new Node<T>(T());
        head.store(dummy);
        tail.store(dummy);
        for (int i = 0; i < MAX_THREADS; ++i) {
            hp[i].ptr.store(nullptr);
            hp[i].active.store(false);
        }
    }

    ~LockFreeQueueHP() {
        while (Node<T>* n = head.load()) {
            head.store(n->next.load());
            delete n;
        }
    }

    void push(T val) {
        Node<T>* new_node = new Node<T>(val);
        Node<T>* last;
        Node<T>* next;
        while (true) {
            last = tail.load();
            next = last->next.load();
            if (last == tail.load()) {
                if (next == nullptr) {
                    if (last->next.compare_exchange_weak(next, new_node)) {
                        break;
                    }
                } else {
                    tail.compare_exchange_weak(last, next);
                }
            }
        }
        tail.compare_exchange_weak(last, new_node);
    }

    bool pop(T& result, int thread_id) {
        hp[thread_id].active.store(true);
        Node<T>* first;
        Node<T>* next;
        while (true) {
            first = head.load();
            hp[thread_id].ptr.store(first);
            if (head.load() != first) continue;
            
            next = first->next.load();
            if (next == nullptr) {
                hp[thread_id].ptr.store(nullptr);
                hp[thread_id].active.store(false);
                return false;
            }

            if (head.compare_exchange_weak(first, next)) {
                result = next->data;
                hp[thread_id].ptr.store(nullptr);
                hp[thread_id].active.store(false);
                retire_node(first);
                return true;
            }
        }
    }
};

int main() {
    LockFreeQueueDeferred<int> q1;
    LockFreeQueueHP<int> q2;

    auto test_q1 = [&]() {
        for (int i = 0; i < 1000; ++i) q1.push(i);
        int val;
        for (int i = 0; i < 1000; ++i) q1.pop(val);
    };

    auto test_q2 = [&]() {
        for (int i = 0; i < 1000; ++i) q2.push(i);
        int val;
        for (int i = 0; i < 1000; ++i) q2.pop(val, 0);
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) threads.emplace_back(test_q1);
    for (auto& t : threads) t.join();
    threads.clear();
    std::cout << "Deferred queue test passed" << std::endl;

    for (int i = 0; i < 4; ++i) threads.emplace_back(test_q2);
    for (auto& t : threads) t.join();
    std::cout << "Hazard pointer queue test passed" << std::endl;

    return 0;
}