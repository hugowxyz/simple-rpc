#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket)
            : socket_(std::move(socket)) {
    }

    void start() {
        doRead();
    }

private:
    void doRead() {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                [this, self](boost::system::error_code ec, std::size_t length) {
                                    std::cout << "asdhusadhui\n";
                                    if (!ec) {
                                        doRead();
                                    }
                                });
    }

    void doWrite(std::size_t length) {
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
                                 [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                     if (!ec) {
                                         doRead();
                                     }
                                 });
    }

    tcp::socket socket_;
    enum {
        max_length = 1024
    };
    char data_[max_length];
};

class Server {
public:
    Server(short port)
            : io_context_{}
            , acceptor_(io_context_, tcp::endpoint(tcp::v4(), port))
    {
        doAccept();
    }

    void run() {
        io_context_.run();
    }

private:
    void doAccept() {
        acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<Session>(std::move(socket))->start();
            }
            doAccept();
        });
    }

private:
    boost::asio::io_context io_context_;
    tcp::acceptor acceptor_;
};

int main() {
    try {
//        boost::asio::io_context io_context;
        Server server(8001);
        server.run();
    }
    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}