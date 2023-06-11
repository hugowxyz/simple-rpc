//
// Created by phanh on 09/06/2023.
//

#ifndef SIMPLERPC_SEPARATE_JSON_H
#define SIMPLERPC_SEPARATE_JSON_H

#include <vector>

#include "nlohmann/json.h"
using json = nlohmann::json;

inline std::vector<json> separate_json(const std::string& jsonString) {
    std::vector<json> separatedObjects;
    std::string buffer;
    int count = 0;

    for (char c : jsonString) {
        buffer += c;

        if (c == '{') {
            count++;
        } else if (c == '}') {
            count--;
        }

        if (count == 0) {
            try {
                json jsonObject = json::parse(buffer);
                separatedObjects.push_back(jsonObject);
            } catch (const json::parse_error&) {
                // Handle invalid JSON object if needed
            }

            buffer.clear();
        }
    }

    return separatedObjects;
}


#endif //SIMPLERPC_SEPARATE_JSON_H
