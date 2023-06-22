#include "simple-rpc/rpc_node.h"

namespace srpc {
    rpc_node::rpc_node(uint16_t port) : server_(port) {}

    std::future<rpc_message> rpc_node::send(conn_info remote, rpc_message message) {
        return client_.send(remote, message);
    }

    void rpc_node::run() {
        server_.run();
    }
}