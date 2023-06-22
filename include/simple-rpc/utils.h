#ifndef SIMPLERPC_UTILS_H
#define SIMPLERPC_UTILS_H

#include <string>
#include <boost/asio.hpp>
#include <boost/functional/hash.hpp>

#include <string>
#include <vector>

#include "nlohmann/json.h"
using json = nlohmann::json;

struct conn_info {
    std::string addr;
    uint16_t port;

    bool operator==(const conn_info& other) const {
        return addr == other.addr && port == other.port;
    }
};

inline conn_info get_remote(boost::asio::ip::tcp::socket& socket) {
    auto remote = socket.remote_endpoint();
    return {
            remote.address().to_string(),
            static_cast<uint16_t>(remote.port())
    };
}

inline conn_info get_local(boost::asio::ip::tcp::socket& socket) {
    auto local = socket.local_endpoint();
    return {
            local.address().to_string(),
            static_cast<uint16_t>(local.port())
    };
}

inline conn_info get_local(boost::asio::ip::tcp::acceptor& acceptor) {
    auto local = acceptor.local_endpoint();
    return {
            local.address().to_string(),
            static_cast<uint16_t>(local.port())
    };
}

namespace std {
    template<>
    struct hash<conn_info> {
        std::size_t operator()(const conn_info& info) const {
            std::size_t seed = 3989;
            boost::hash_combine(seed, info.addr);
            boost::hash_combine(seed, info.port);
            return seed;
        }
    };
}


#endif //SIMPLERPC_UTILS_H
