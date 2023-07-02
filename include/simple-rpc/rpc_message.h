//
// Created by phanh on 08/06/2023.
//

#ifndef SIMPLERPC_RPC_MESSAGE_H
#define SIMPLERPC_RPC_MESSAGE_H

#include <string>
#include <utility>
#include <vector>
#include <future>
#include "utils.h"
#include <boost/asio.hpp>
#include <chrono>
#include <iostream>

#include "nlohmann/json.h"
using json = nlohmann::json;

enum class RpcStatusCode {
    Placeholder,
    Success,
    GenericError,
    InvalidRequest,
    Unauthorized,
    ResourceNotFound,
    MethodNotSupported,
    Timeout,
    ServerError,
    RateLimitExceeded,
    NetworkError
};

class rpc_message {
public:
    std::string message_id;
    conn_info source;
    conn_info destination;
    std::unordered_map<std::string, std::string> headers{};

    RpcStatusCode status_code = RpcStatusCode::Placeholder;
    std::string procedure_name;

    std::string payload;

    rpc_message() = default;
    rpc_message(RpcStatusCode status_code_, std::string payload_)
        : status_code(status_code_)
        , payload(std::move(payload_)) {}

    static rpc_message make_reply(rpc_message message, RpcStatusCode status_code_, std::string payload_) {
        rpc_message reply = std::move(message);
        reply.status_code = status_code_;
        reply.payload = std::move(payload_);
        return reply;
    }

    json to_json() {
        return json{
                {"message_id",     message_id},
                {"source",         {
                                           {"address", source.addr},
                                           {"port", source.port}
                                   }},
                {"destination",    {
                                           {"address", destination.addr},
                                           {"port", destination.port}
                                   }},
                {"status_code",    static_cast<int>(status_code)},
                {"procedure_name", procedure_name},
                {"payload",        payload},
                {"headers",        headers}
        };
    }

    static rpc_message from_json(json &j) {
//        std::cout << j << std::endl;
        rpc_message m;
        m.message_id = j["message_id"];
        m.source = {
                j["source"]["address"],
                j["source"]["port"]
        };
        m.destination = {
                j["destination"]["address"],
                j["destination"]["port"]
        };
        m.status_code = static_cast<RpcStatusCode>(j["status_code"]);
        m.procedure_name = j["procedure_name"];
        m.payload = j["payload"];
        m.headers = j["headers"];

        return m;
    }
};

#endif //SIMPLERPC_RPC_MESSAGE_H
