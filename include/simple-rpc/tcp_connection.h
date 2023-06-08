#pragma once

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace srpc {

    class tcp_connection
            : public std::enable_shared_from_this<tcp_connection> {
    public:
        tcp_connection(tcp::socket socket)
                : socket_(std::move(socket)) {
        }

        //! action on connection
        void start();

    private:
        void do_read();

    private:
        tcp::socket socket_;

        static constexpr uint32_t BUFFER_SIZE = 4096;
        char buffer_[BUFFER_SIZE];
    };

}