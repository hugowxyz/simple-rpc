//
// Created by phanh on 09/06/2023.
//

#ifndef SIMPLERPC_SEPARATE_JSON_H
#define SIMPLERPC_SEPARATE_JSON_H

#include <vector>

#include "nlohmann/json.h"
using json = nlohmann::json;

inline std::vector<json> separate_json(const std::string& input, int& idx) {
    std::vector<json> separated_objects;
    std::string buffer;
    int start = 0;

    while (start < input.length()) {
        size_t json_start = input.find('{', start);
        if (json_start == std::string::npos) {
            break;  // No more JSON objects found
        }

        size_t json_end = -1;
        int count = 0;
        for (int i=json_start; i<input.size(); i++) {
            char c = input[i];
            if (c == '{') {
                count++;
            } else if (c == '}') {
                count--;
            }

            if (count == 0) {
                json_end = i;
                break;
            }
        }

        if (json_end == -1) {
            break;  // Invalid JSON, exit the loop
        }


        idx = json_end;

        std::string json_str = input.substr(json_start, json_end - json_start + 1);
        json jsonObject = json::parse(json_str);
        separated_objects.push_back(jsonObject);

        start = json_end + 1;
    }

    return separated_objects;
}

inline std::vector<json> separate_json(const std::string& input) {
    int idx;
    return separate_json(input, idx);
}



#endif //SIMPLERPC_SEPARATE_JSON_H
