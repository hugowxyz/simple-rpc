#pragma once

#include "rpc_message.h"
#include <functional>
#include <unordered_map>

namespace srpc {

class dispatcher {
public:
    using function_type = std::function<rpc_message(rpc_message)>;
    void bind(std::string name, function_type func) {
        if (handlers_.count(name) > 0) {
            throw std::runtime_error("function name already bounded");
        }

        handlers_[name] = func;
    }

    rpc_message dispatch(const std::string& name, rpc_message message) {
        if (handlers_.count(name) == 0) {
            message.status_code = RpcStatusCode::MethodNotSupported;
            message.payload = "Requested procedure has not been bound";
            return message;
        }

        return handlers_[name](message);
    }

    /**
     * what is a node in the cluster? both a listener and a sender.
     * a node will be built on this RPC framework probably.
     *
     * a node should
     *
     * auto node = new rpc_node()
     * node.bind("broadcast", [&node](rpc_message message) {
     *      auto fut = node::send(conn_info, payload);
     *
     *      return ...
     * })
     */

private:
    std::unordered_map<std::string, function_type> handlers_;
};

}