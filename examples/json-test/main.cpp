#include <iostream>
#include <string>
#include <vector>

#include "nlohmann/json.h"
using json = nlohmann::json;

#include "simple-rpc/separate_json.h"

int main(int argc, char* argv[]) {
    if (argc > 0) {
        auto res = separate_json(argv[1]);
        for (auto i : res) {
            std::cout << i << std::endl;
        }
    }

    return 0;
}