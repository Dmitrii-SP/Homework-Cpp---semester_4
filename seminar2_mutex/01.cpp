#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <algorithm>
#include <cctype>

std::string global_line = "";
std::mutex mtx;
bool file_read_done = false;

void producer(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        file_read_done = true;
        return;
}

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        while (true) {
            std::lock_guard<std::mutex> lock(mtx);
            if (global_line.empty()) {
                global_line = line;
                break;
            }
        }
    }

    while (true) {
        std::lock_guard<std::mutex> lock(mtx);
        if (global_line.empty()) {
            file_read_done = true;
            break;
        }
    }
}

void consumer() {
    while (true) {
        std::string current_line = "";
        bool done = false;

        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!global_line.empty()) {
                current_line = global_line;
                global_line.clear();
            } else if (file_read_done) {
                done = true;
            }
        }

        if (done) {
            break;
        }

        if (!current_line.empty()) {
            std::transform(current_line.begin(), current_line.end(), current_line.begin(), [](unsigned char c) {
                return std::toupper(c);
            });
            std::cout << current_line << std::endl;
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