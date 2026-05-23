#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <cctype>

std::mutex mtx;
std::condition_variable cv_producer; 
std::condition_variable cv_consumer; 
std::string shared_line;             
bool data_ready = false;             
bool finished = false;               

void reader_thread(const std::string& filename) {
    std::ifstream file(filename);

    std::string line;
    while (std::getline(file, line)) {
        std::unique_lock<std::mutex> lock(mtx);
        
        cv_producer.wait(lock, [] { return !data_ready; });

        shared_line = line;
        data_ready = true;
        
        cv_consumer.notify_one();
    }

    std::unique_lock<std::mutex> lock(mtx);
    finished = true;
    cv_consumer.notify_one();
}

void processor_thread() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        
        cv_consumer.wait(lock, [] { return data_ready || finished; });

        if (finished && !data_ready) {
            break;
        }

        std::string current_line = shared_line;
        std::transform(current_line.begin(), current_line.end(), current_line.begin(), ::toupper);
        
        std::cout << current_line << std::endl;

        shared_line.clear();
        data_ready = false;
        
        cv_producer.notify_one();
    }
}

int main() {
    std::string filename = "invisible_man.txt";

    std::thread t1(reader_thread, filename);
    std::thread t2(processor_thread);

    t1.join();
    t2.join();

    return 0;
}