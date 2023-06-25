#pragma once

#include <string>
#include <thread>
#include <future>
#include <memory>
#include <unordered_set>
#include <unordered_map>

#include <boost/asio.hpp>
#include <utility>

#include "id-generators/abstract_id_generator.h"
#include "id-generators/uuid_generator.h"

#include "rpc_message.h"
#include "utils.h"

#include "separate_json.h"

#define FMT_HEADER_ONLY
#include "fmt/core.h"

#include "nlohmann/json.h"
using json = nlohmann::json;

using boost::asio::ip::tcp;

namespace srpc {
    using pending_promise = std::shared_ptr<std::promise<rpc_message>>;

    struct message_store {
        std::string id_;
        pending_promise promise_;
        conn_info remote_;
    };

    class pending_info {
    public:
        void insert(message_store message) {
            pending_messages_.insert({message.id_, message});
            if (open_connections_.count(message.remote_) == 0) {
                open_connections_.insert({message.remote_, {}});
            }

            open_connections_[message.remote_].push_back(message.id_);
        }

        void erase(const std::string& message_id) {
            auto remote = pending_messages_[message_id].remote_;
            auto new_end = std::remove(open_connections_[remote].begin(), open_connections_[remote].end(), message_id);
            open_connections_[remote].erase(new_end, open_connections_[remote].end());
//            pending_data_.erase(message_id);
        }

        unsigned long open_connections(const conn_info& remote) {
            if (open_connections_.count(remote) <= 0) {
                return 0;
            }

            return open_connections_[remote].size();
        }

        pending_promise get_promise(const std::string& message_id) {
            if (pending_messages_.count(message_id) <= 0) {
                throw std::runtime_error("message id doesn't exist in pending_messages");
            }

            return pending_messages_[message_id].promise_;
        }

    private:
        std::unordered_map<std::string, message_store> pending_messages_;
        std::unordered_map<conn_info, std::vector<std::string>> open_connections_;
    };

    class client {
    public:
        client();
        ~client();

        std::future<rpc_message> send(conn_info remote, rpc_message message) {
            std::shared_ptr<tcp::socket> socket;
            if (sockets_.count(remote) <= 0) {
                socket = do_connect(remote);
            } else {
                socket = sockets_[remote];
            }

            std::string message_id = id_generator_->generate_id();

            message.message_id = message_id;
            message.source = get_local(*socket);
            message.destination = remote;

            auto promise = std::make_shared<std::promise<rpc_message>>();
            auto future = promise->get_future();

            pending_data_.insert({message_id, promise, get_remote(*socket) });

            std::string data = message.to_json().dump();
            socket->async_write_some(
                    boost::asio::buffer(data),
                    [this](boost::system::error_code error, size_t bytes) {});

            return future;
        }

    private:
        std::shared_ptr<tcp::socket> do_connect(const conn_info& remote) {
            std::cout << "Establishing new socket connection with " << remote.addr << " " << remote.port << std::endl;

            tcp::resolver resolver(io_);
            auto endpoint_iterator = resolver.resolve({remote.addr, std::to_string(remote.port)});
            auto socket = std::make_shared<tcp::socket>(io_);
            sockets_.insert({remote, socket});
            boost::asio::connect(*socket, endpoint_iterator);
            do_read(socket);

            return socket;
        }

        void do_read(const std::shared_ptr<tcp::socket>& socket) {
            socket->async_read_some(
                    boost::asio::buffer(buffer_, BUFFER_SIZE),
                    [this, socket](boost::system::error_code error, size_t bytes) {
                        if (!error) {
                            std::string data(buffer_, bytes);
                            auto v = separate_json(data);

                            for (auto &json_message : v) {
                                rpc_message message = rpc_message::from_json(json_message);
                                if (pending_data_.get_promise(message.message_id) != nullptr) {
                                    pending_data_.get_promise(message.message_id)->set_value(message);
                                }
                            }

                            memset(buffer_, 0, BUFFER_SIZE);

                            if (pending_data_.open_connections(get_remote(*socket)) <= 0) {
                                sockets_.erase(get_remote(*socket));
                                fmt::print("No more active RPC calls... ending connection\n");
                                return;
                            }

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
        std::unordered_map<conn_info, std::shared_ptr<tcp::socket>> sockets_;
        tcp::socket socket_;

        boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_;
        std::thread event_loop_thread_;

        static const uint32_t BUFFER_SIZE = 4096;
        char buffer_[BUFFER_SIZE]{};

        abstract_id_generator *id_generator_ = new uuid_generator();
        pending_info pending_data_;
    };

}
