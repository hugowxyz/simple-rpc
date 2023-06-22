//
// Created by phanh on 11/06/2023.
//

#ifndef SIMPLERPC_RPC_NODE_H
#define SIMPLERPC_RPC_NODE_H

#include "server.h"
#include "client.h"

namespace srpc {
    class rpc_node {
    public:
        rpc_node(uint16_t port);

        using function_type = std::function<rpc_message(rpc_message)>;
        void bind(std::string name, function_type func) {
            server_.bind(name, func);
        }

        std::future<rpc_message> send(conn_info, rpc_message);

        void run();

    private:
        server server_;
        client client_;
    };
}


#endif //SIMPLERPC_RPC_NODE_H
