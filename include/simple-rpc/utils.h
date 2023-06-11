#ifndef SIMPLERPC_UTILS_H
#define SIMPLERPC_UTILS_H

#include <string>
#include <boost/asio.hpp>

#include <string>
#include <vector>

#include "nlohmann/json.h"
using json = nlohmann::json;

struct conn_info {
    std::string addr;
    uint16_t port;
};

#endif //SIMPLERPC_UTILS_H
