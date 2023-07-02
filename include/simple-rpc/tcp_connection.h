#pragma once

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <utility>
#include "utils.h"
#include "dispatcher.h"

using boost::asio::ip::tcp;

namespace srpc {

    class tcp_connection
            : public std::enable_shared_from_this<tcp_connection> {
    public:
        tcp_connection(tcp::socket socket, conn_info source, std::shared_ptr<dispatcher> dispatcher)
                : socket_(std::move(socket))
                , source_(std::move(source))
                , dispatcher_(dispatcher)
        {
        }

        //! action on connection
        void start();

    private:
        void do_read();
        void send_message(rpc_message message);

    private:
        conn_info source_;
        tcp::socket socket_;

        static constexpr uint32_t BUFFER_SIZE = 4096;
        std::vector<char> buffer_;
//        char buffer_[BUFFER_SIZE]{};
        char temp_buffer_[BUFFER_SIZE]{};
        std::shared_ptr<dispatcher> dispatcher_;
    };

}