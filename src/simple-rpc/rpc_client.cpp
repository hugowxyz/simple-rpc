#include <utility>

#include "simple-rpc/client.h"
#include "simple-rpc/rpc_client.h"

namespace srpc {

    rpc_client::rpc_client()
            : pending_messages_{}
    {
        id_generator_ = new sequential_id_generator();
        pending_messages_ = new std::unordered_map<std::string, std::promise<rpc_message>*>();
    }

    rpc_client::~rpc_client() {
        for (const auto& it : *pending_messages_) {
            delete it.second;
        }

        pending_messages_->clear();
    }

//    template<typename Callable>
//    std::future<rpc_message> rpc_client::send(rpc_message message, Callable &&handler) {
//        std::string id = id_generator_->generate_id();
//        message.message_id = id;
//        auto endpoint = socket_.local_endpoint();
//        message.source = {
//                endpoint.address().to_string(),
//                static_cast<uint16_t>(endpoint.port())
//        };
//        message.destination = connection_info_;
//
//        auto promise = std::make_unique<std::promise<rpc_message>>();
//        auto future = promise->get_future();
//
//        std::string data = message.to_json().dump();
//        client::send(data, [this, &id, &promise, &handler](boost::system::error_code error, std::size_t bytes) {
//            pending_messages_[id] = std::move(promise);
//            handler(error, bytes);
//        });
//
//        return future;
//    }

//    void rpc_client::connect(conn_info ci) {
//        client::connect(std::move(ci), [this](char *buffer, boost::system::error_code error, std::size_t bytes) {
//            std::string data(buffer);
//            json json_message = json::parse(data);
//            rpc_message message = rpc_message::from_json(json_message);
//            auto sz = pending_messages_->size();
//            auto res = (*pending_messages_)[message.message_id];
//            if (res != nullptr)
//                    res->set_value(message);
////            pending_messages_.erase(message.message_id);
//        });
//    }
}