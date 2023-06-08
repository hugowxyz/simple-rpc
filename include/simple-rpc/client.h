#pragma once

#include <string>
#include <thread>
#include <memory>
#include <boost/asio.hpp>

#include "utils.h"

#define FMT_HEADER_ONLY
#include "fmt/core.h"

using boost::asio::ip::tcp;

namespace srpc {

    class client {
    public:
        client();
        ~client();

        template <typename Callable>
        void send(const std::string &message, Callable&& handler) {
            if (!connected_) {
                fmt::print("Warning client not connected...\n");
            }

            fmt::print("Client sending buffer: {}\n", message);
            socket_.async_write_some(
                    boost::asio::buffer(message),
                    [this, &handler](boost::system::error_code error, size_t bytes) {
                        handler(error, bytes);
                    });
        }

        template <typename Callable>
        void connect(conn_info cinfo, Callable&& cb) {
            connection_info_ = std::move(cinfo);
            do_connect(cb);
        }

        conn_info &current_connection() { return connection_info_; }

    private:
        template <typename Callable>
        void do_connect(Callable&& cb) {
            tcp::resolver resolver(io_);
            auto endpoint_iterator = resolver.resolve({connection_info_.addr, std::to_string(connection_info_.port)});
            boost::asio::connect(socket_, endpoint_iterator);
            do_read(cb);

            connected_ = true;
        }

        template <typename Callable>
        void do_read(Callable&& handler) {
            socket_.async_read_some(
                    boost::asio::buffer(buffer_, BUFFER_SIZE),
                    [this, &handler](boost::system::error_code error, size_t bytes) {
                        fmt::print("Client read {} bytes. Buffer={}\n", bytes, buffer_);
                        handler(buffer_, error, bytes);
                        if (!error) {
                            do_read(handler);
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

    protected:
        boost::asio::io_context io_;
        tcp::socket socket_;
        conn_info connection_info_;

    private:
        std::atomic<bool> connected_;

        boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_;
        std::thread event_loop_thread_;

        static const uint32_t BUFFER_SIZE = 4096;
        char buffer_[BUFFER_SIZE]{};


    };
}
