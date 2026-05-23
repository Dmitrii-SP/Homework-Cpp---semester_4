#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <future>
#include <chrono>
#include <cctype>

using namespace std::chrono_literals;

struct ColorAccumulator {
    unsigned long long r = 0, g = 0, b = 0;
};

std::string extractNextToken(std::istream& stream) {
    std::string token;
    char ch;
    while (stream.get(ch)) {
        if (std::isspace(static_cast<unsigned char>(ch))) continue;
        if (ch == '#') {
            std::string dummy;
            std::getline(stream, dummy);
            continue;
        }
        token += ch;
        break;
    }
    while (stream.get(ch)) {
        if (std::isspace(static_cast<unsigned char>(ch)) || ch == '#') {
            stream.unget();
            break;
        }
        token += ch;
    }
    return token;
}

int readBinaryChannel(std::istream& stream, int max_val) {
    unsigned char first, second;
    if (max_val <= 255) {
        if (!stream.get(reinterpret_cast<char&>(first))) return -1;
        return first;
    }
    if (!stream.get(reinterpret_cast<char&>(first)) || !stream.get(reinterpret_cast<char&>(second))) 
        return -1;
    return (first << 8) | second;
}

std::string processImageFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return "File " + filename + " does not exist";

    std::string format = extractNextToken(file);
    int width = std::stoi(extractNextToken(file));
    int height = std::stoi(extractNextToken(file));
    int max_val = std::stoi(extractNextToken(file));

    if ((format != "P3" && format != "P6") || width <= 0 || height <= 0 || max_val <= 0)
        return "File " + filename + " has wrong format";

    ColorAccumulator totals;
    unsigned long long pixel_count = static_cast<unsigned long long>(width) * height;

    if (format == "P3") {
        for (unsigned long long i = 0; i < pixel_count; ++i) {
            totals.r += std::stoi(extractNextToken(file));
            totals.g += std::stoi(extractNextToken(file));
            totals.b += std::stoi(extractNextToken(file));
        }
    } else {
        char next;
        file.get(next);
        if (next == '\r' && file.peek() == '\n') file.get(next);

        for (unsigned long long i = 0; i < pixel_count; ++i) {
            int r = readBinaryChannel(file, max_val);
            int g = readBinaryChannel(file, max_val);
            int b = readBinaryChannel(file, max_val);
            
            if (r < 0 || g < 0 || b < 0) return "File " + filename + " has corrupted data";
            totals.r += r; totals.g += g; totals.b += b;
        }
    }

    return "Average color of " + filename + " is: (" +
           std::to_string(totals.r / pixel_count) + ", " +
           std::to_string(totals.g / pixel_count) + ", " +
           std::to_string(totals.b / pixel_count) + ")";
}

void pollResults(std::vector<std::future<std::string>>& active_tasks) {
    auto it = active_tasks.begin();
    while (it != active_tasks.end()) {
        if (it->wait_for(0ms) == std::future_status::ready) {
            std::cout << it->get() << std::endl;
            it = active_tasks.erase(it);
        } else {
            ++it;
        }
    }
}

int main() {
    std::vector<std::future<std::string>> pending_tasks;
    std::string input;

    while (true) {
        pollResults(pending_tasks);

        std::cout << "Enter image name: ";
        std::cin >> input;

        if (input == "quit") break;

        pending_tasks.emplace_back(std::async(std::launch::async, processImageFile, input));
    }

    for (auto& task : pending_tasks) {
        std::cout << task.get() << std::endl;
    }
    
    return 0;
}


// Enter image name: mipt.ppm
// Average color of mipt.ppm is: (104, 129, 137)