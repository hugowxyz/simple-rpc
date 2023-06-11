#pragma once

#include <string>
#include <thread>
#include <future>
#include <memory>

#include <boost/asio.hpp>

#include "id-generators/abstract_id_generator.h"
#include "id-generators/sequential_id_generator.h"

#include "rpc_message.h"
#include "utils.h"

#include "separate_json.h"

#define FMT_HEADER_ONLY
#include "fmt/core.h"

#include "nlohmann/json.h"
using json = nlohmann::json;

using boost::asio::ip::tcp;

namespace srpc {
    class client {
    public:
        client();
        ~client();

        std::future<rpc_message> send(rpc_message message) {
            std::string message_id = id_generator_->generate_id();

            message.message_id = message_id;
            auto endpoint = socket_.local_endpoint();
            message.source = {
                    endpoint.address().to_string(),
                    static_cast<uint16_t>(endpoint.port())
            };
            message.destination = connection_info_;

            auto promise = std::make_shared<std::promise<rpc_message>>();
            auto future = promise->get_future();

            pending_messages_.insert({ message_id, promise });

            std::string data = message.to_json().dump();
            socket_.async_write_some(
                    boost::asio::buffer(data),
                    [this](boost::system::error_code error, size_t bytes) {

                    });

            return future;
        }

        void connect(conn_info cinfo) {
            connection_info_ = std::move(cinfo);
            do_connect();
        }

        conn_info &current_connection() { return connection_info_; }

    private:
        void do_connect() {
            tcp::resolver resolver(io_);
            auto endpoint_iterator = resolver.resolve({connection_info_.addr, std::to_string(connection_info_.port)});
            boost::asio::connect(socket_, endpoint_iterator);
            boost::asio::ip::tcp::no_delay option(true);
            socket_.set_option(option);
            do_read(socket_);

            connected_ = true;
        }

        void do_read(tcp::socket& socket) {
            socket.async_read_some(
                    boost::asio::buffer(buffer_, BUFFER_SIZE),
                    [this, &socket](boost::system::error_code error, size_t bytes) {
                        if (!error) {
                            std::string data(buffer_, bytes);
                            auto v = separate_json(data);

                            for (auto &json_message : v) {
                                rpc_message message = rpc_message::from_json(json_message);
                                if (pending_messages_[message.message_id] != nullptr)
                                    pending_messages_[message.message_id]->set_value(message);

                            }

                            memset(buffer_, 0, BUFFER_SIZE);
                            do_read(socket);
                        } else {
                            if (error == boost::asio::error::eof) {
                                fmt::print("Connection has been closed by peer\n");
                            } else {
                                fmt::print("Error encountered {}\n", error.message());
                            }
                        }
                    });
        }

        void initialize();

    private:
        boost::asio::io_context io_;
        tcp::socket socket_;
        conn_info connection_info_;

        std::atomic<bool> connected_;

        boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_;
        std::thread event_loop_thread_;

        static const uint32_t BUFFER_SIZE = 4096;
        char buffer_[BUFFER_SIZE]{};

        abstract_id_generator *id_generator_ = new sequential_id_generator();
        std::unordered_map<std::string, std::shared_ptr<std::promise<rpc_message>>> pending_messages_{};
    };

}
