#include "simple-rpc/rpc_node.h"

#include <iostream>
#include <thread>
#include <future>
#include <vector>
#include <string>
#include <random>

using namespace srpc;

class broadcast_node : public rpc_node {
public:
    explicit broadcast_node(uint16_t port) : rpc_node(port) {}

//    std::vector<int> values;
};

int main() {
    broadcast_node node(8000);

    node.bind("add", [&node](const rpc_message& message) {
        return rpc_message::make_reply(message, RpcStatusCode::Success, "broadcast_ok");
    });


    node.run();
}