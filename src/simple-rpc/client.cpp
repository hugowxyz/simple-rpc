#include "simple-rpc/client.h"

#include <utility>

#define FMT_HEADER_ONLY

#include "fmt/core.h"

namespace srpc {
    client::client()
            : io_(), socket_(io_), work_guard_(boost::asio::make_work_guard(io_)) {
        initialize();
    }

    client::~client() {
        fmt::print("Client destructor\n");
        io_.stop();
        event_loop_thread_.join();
    }

    void client::initialize() {
        std::thread ev_thread([this]() {
            io_.run();
        });
        event_loop_thread_ = std::move(ev_thread);
    }

//    template <typename Callable>
//    void client::connect(conn_info cinfo, Callable&& cb) {
//        connection_info_ = std::move(cinfo);
//        do_connect(cb);
//    }

//    template <typename Callable>
//    void client::do_connect(Callable&& cb) {
//        tcp::resolver resolver(io_);
//        auto endpoint_iterator = resolver.resolve({connection_info_.addr, std::to_string(connection_info_.port)});
//        boost::asio::connect(socket_, endpoint_iterator);
//        do_read(cb);
//
//        connected_ = true;
//    }

//    template <typename Callable>
//    void client::do_read(Callable&& handler) {
//        socket_.async_read_some(
//                boost::asio::buffer(buffer_, BUFFER_SIZE),
//                [this, &handler](boost::system::error_code error, size_t bytes) {
//                    fmt::print("Client read {} bytes. Buffer={}\n", bytes, buffer_);
//                    handler(buffer_, error, bytes);
//                    if (!error) {
//                        do_read(handler);
//                    } else {
//                        if (error == boost::asio::error::eof) {
//                            fmt::print("Connection has been closed by peer\n");
//                        } else {
//                            fmt::print("Error encountered {}\n", error.message());
//                        }
//                    }
//                });
//    }

//    template <typename Callable>
//    void client::send(const std::string &message, Callable&& handler) {
//        if (!connected_) {
//            fmt::print("Warning client not connected...\n");
//        }
//
//        fmt::print("Client sending buffer: {}\n", message);
//        socket_.async_write_some(
//                boost::asio::buffer(message),
//                [this, &handler](boost::system::error_code error, size_t bytes) {
//                    handler(error, bytes);
//                });
//    }

}