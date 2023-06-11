//
// Created by phanh on 08/06/2023.
//

#ifndef SIMPLERPC_RPC_MESSAGE_H
#define SIMPLERPC_RPC_MESSAGE_H

#include <string>
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
