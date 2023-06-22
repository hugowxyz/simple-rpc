

#define FMT_HEADER_ONLY

#include "fmt/core.h"

#include <boost/asio.hpp>

#include "simple-rpc/tcp_connection.h"
#include "simple-rpc/server.h"

using boost::asio::ip::tcp;

namespace srpc {

    server::server(unsigned short port)
            : io_()
            , acceptor_(io_, tcp::endpoint(boost::asio::ip::tcp::v4(), port))
            , source_(get_local(acceptor_))
            , dispatcher_(std::make_shared<dispatcher>())
    {
        fmt::print("Starting server on address {}:{}\n", source_.addr, source_.port);
        do_accept();
    }

    server::~server() {
        fmt::print("Server destructor\n");
        io_.stop();
        event_loop_thread_.join();
    }

    void server::run() {
        io_.run();
    }

    void server::run_async() {
        std::thread ev_thread([this]() {
            io_.run();
        });
        event_loop_thread_ = std::move(ev_thread);
    }

    void server::do_accept() {
        acceptor_.async_accept(
                [this](const boost::system::error_code &error, tcp::socket socket) {
                    if (!error) {
                        fmt::print("Accepted connection\n");

                        std::make_shared<tcp_connection>(std::move(socket), source_, dispatcher_)->start();
                    } else {
                        fmt::print("Error accepting connection: {}\n", error.message());
                    }

                    do_accept();
                });
    }
}