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
//        int a;
//        try {
//            a = std::stoi(message.payload);
//        } catch (const std::exception& e) {
//            std::cout << e.what() << std::endl;
//            return rpc_message(RpcStatusCode::InvalidRequest, e.what());
//        }

//        if (node.values.size() > 0) {
//            if (node.values.back() >= a) {
//                return rpc_message(RpcStatusCode::Success, "message in the past");
//            }
//        }

//        node.values.push_back(a);
//        node.send({"0.0.0.0", 8000}, message);

        return rpc_message::make_reply(message, RpcStatusCode::Success, "broadcast_ok");
    });

//    node.bind("get", [&node](const rpc_message& message) {
//        return rpc_message::make_reply(message, RpcStatusCode::Success, json(node.values).dump());
//    });

    node.run();
}