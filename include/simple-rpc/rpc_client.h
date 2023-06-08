#ifndef SIMPLERPC_RPC_CLIENT_H
#define SIMPLERPC_RPC_CLIENT_H

#include <unordered_map>
#include "utils.h"
#include "id-generators/abstract_id_generator.h"
#include "id-generators/sequential_id_generator.h"
#include <string>
#include <vector>
#include <future>
#include "nlohmann/json.h"
#include "utils.h"
#include "rpc_client.h"
#include <boost/asio.hpp>
#include <chrono>
#include "client.h"
#include <iostream>

using json = nlohmann::json;

namespace srpc {
    class rpc_client;

    enum class RpcErrorCode {
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
    private:
        std::string message_id;
        conn_info source;
        conn_info destination;

        friend class rpc_client;
    public:
        RpcErrorCode status_code = RpcErrorCode::Placeholder;
        std::string procedure_name;
        std::string error_message;
        std::string payload;
        std::unordered_map<std::string, std::string> headers{};

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
                    {"error_message",  error_message},
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
            m.status_code = static_cast<RpcErrorCode>(j["status_code"]);
            m.procedure_name = j["procedure_name"];
            m.error_message = j["error_message"];
            m.payload = j["payload"];
            m.headers = j["headers"];

            return m;
        }
    };

    class rpc_client : private client {
    public:
        using rpc_message_id = std::string;

        rpc_client();
        ~rpc_client();

        void connect(conn_info ci) {
            client::connect(std::move(ci), [this](char *buffer, boost::system::error_code error, std::size_t bytes) {
                std::string data(buffer);
                json json_message = json::parse(data);
                rpc_message message = rpc_message::from_json(json_message);
                std::cout << json_message << std::endl;

                auto sz = pending_messages_->size();
                std::cout << sz << std::endl;
            });
        }

        template <typename Callable>
        std::future<rpc_message> send(rpc_message message, Callable&& handler) {
            std::string message_id = id_generator_->generate_id();

            message.message_id = message_id;
            auto endpoint = socket_.local_endpoint();
            message.source = {
                    endpoint.address().to_string(),
                    static_cast<uint16_t>(endpoint.port())
            };
            message.destination = connection_info_;

            auto *promise = new std::promise<rpc_message>();
            auto future = promise->get_future();
            pending_messages_->insert({ message_id, promise });

            std::string data = message.to_json().dump();
            client::send(data, [this, &handler](boost::system::error_code error, std::size_t bytes) {
                handler(error, bytes);
            });

            return future;
        }


    private:
        abstract_id_generator *id_generator_;
        std::unordered_map<rpc_message_id, std::promise<rpc_message>*> *pending_messages_;
    };


}

#endif //SIMPLERPC_RPC_CLIENT_H
