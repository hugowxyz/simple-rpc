#ifndef SIMPLERPC_UTILS_H
#define SIMPLERPC_UTILS_H

#include <string>
#include <boost/asio.hpp>

struct conn_info {
    std::string addr;
    uint16_t port;
};

//void empty_boost_callback(boost::system::error_code, std::size_t) {}

#endif //SIMPLERPC_UTILS_H
