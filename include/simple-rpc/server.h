#pragma once

#include <thread>
#include <boost/asio.hpp>

namespace srpc {

class server {
public:
    server(std::string const &address, unsigned short port);
    ~server();

    //! Starts synchronous (blocking) server loop
    void run();
    void run_async();

private:
    //! Attach connection handlers
    void do_accept();

private:
    std::thread event_loop_thread_;

    boost::asio::io_context io_;
    boost::asio::ip::tcp::acceptor acceptor_;
};

}