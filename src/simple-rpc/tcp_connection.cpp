#include "simple-rpc/tcp_connection.h"

#define FMT_HEADER_ONLY

#include "fmt/core.h"

namespace srpc {

    void tcp_connection::start() {
        fmt::print("tcp connection start\n");
        do_read();
    }

    void tcp_connection::do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(
                boost::asio::buffer(buffer_, BUFFER_SIZE),
                [this, self](const boost::system::error_code &error, size_t bytes) {
                    fmt::print("Server read {} bytes. Buffer = {}\n", bytes, buffer_);
                    if (!error) {
                        socket_.async_write_some(
                                boost::asio::buffer(buffer_),
                                [this](boost::system::error_code ec, std::size_t sz) {});
                        memset(buffer_, 0, BUFFER_SIZE);
                        do_read();
                    } else {
                        fmt::print("Error reading from socket: {}", error.message());
                    }
                }
        );
    }

}