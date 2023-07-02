#include <iostream>
#include <vector>
#include <string>
#include "simple-rpc/separate_json.h"

std::vector<char> buffer;

void fun(const char* temp_buffer, int bytes) {
    buffer.reserve(buffer.size() + bytes);
    buffer.insert(buffer.end(), temp_buffer, temp_buffer + bytes);

    int idx = -1;
    std::string s(std::string(buffer.begin(), buffer.end()));
    auto res = separate_json(s, idx);

    for (auto& r : res) {
        std::cout << "res: " << res << std::endl;
    }

    if (idx > 0) {
        buffer.erase(buffer.begin(), buffer.begin() + idx + 1);
    }

}

int main() {
    std::string buffer = R"({"key1": "value1", "key2": {"nested_key1": "nested_value1", "nested_key2": {"deeply_nested_key": "deeply_nested_value"}}}{"key1": "value1", "key2": {"nested_key1": "nested_value1", "nested_key2": {"deeply_nested_key": "deeply_nested_value"}}})";
    int bytes = 10;
    for (int i=0; i < buffer.size(); i+=bytes) {
        auto s = buffer.substr(i, bytes);
        const char* c = s.c_str();
        fun(c, s.size());
    }
}