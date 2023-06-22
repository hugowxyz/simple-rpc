#pragma once

#include <thread>
#include <unordered_map>
#include <boost/asio.hpp>
#include "dispatcher.h"

namespace srpc {

class server {
public:
    server(unsigned short port);
    ~server();

    //! Starts synchronous (blocking) server loop
    void run();
    void run_async();

    using function_type = std::function<rpc_message(rpc_message)>;
    void bind(std::string name, function_type func) {
        dispatcher_->bind(name, func);
    }


private:
    //! Attach connection handlers
    void do_accept();

private:
    std::thread event_loop_thread_;

    boost::asio::io_context io_;
    boost::asio::ip::tcp::acceptor acceptor_;
    conn_info source_;
    std::shared_ptr<dispatcher> dispatcher_;
};

}