#define FMT_HEADER_ONLY
#include "fmt/core.h"

#include "simple-rpc/tcp_connection.h"
#include "simple-rpc/rpc_message.h"
#include "simple-rpc/separate_json.h"

namespace srpc {
    void tcp_connection::start() {
        fmt::print("tcp connection start\n");
        do_read();
    }

    rpc_message construct_invalid_message() {
        rpc_message invalid_request_msg;
        invalid_request_msg.status_code = RpcStatusCode::InvalidRequest;
        invalid_request_msg.procedure_name = "";
        invalid_request_msg.payload = "Request was not valid JSON";

        return invalid_request_msg;
    }

    void send_message(rpc_message& message) {

    }

    void tcp_connection::do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(
                boost::asio::buffer(temp_buffer_, BUFFER_SIZE),
                [this, self](const boost::system::error_code &error, size_t bytes) {
                    if (!error) {
                        boost::system::error_code ec;
                        auto remote_info = socket_.remote_endpoint(ec);

                        if (ec) {
                            std::cerr << "[Error] Failed to fetch remote_endpoint: " << ec.message() << std::endl;
                            std::cerr << "Socket is_open " << socket_.is_open() << std::endl;
                            return;
                        }

                        buffer_.reserve(buffer_.size() + bytes);
                        buffer_.insert(buffer_.end(), temp_buffer_, temp_buffer_ + bytes);

                        int idx = -1;
                        std::string data(buffer_.begin(), buffer_.end());
                        auto v = separate_json(data, idx);

                        if (idx > 0) {
                            buffer_.erase(buffer_.begin(), buffer_.begin() + idx + 1);
                        }

                        conn_info dst = {
                                remote_info.address().to_string(),
                                static_cast<uint16_t>(remote_info.port())
                        };

                        for (auto &json_message : v) {
                            rpc_message message;
                            try {
                                message = rpc_message::from_json(json_message);
                            } catch (const std::exception& e) {
                                rpc_message invalid_message;
                                invalid_message.message_id = message.message_id;
                                invalid_message.source = source_;
                                invalid_message.destination = dst;
                                invalid_message.status_code = RpcStatusCode::InvalidRequest;
                                invalid_message.payload = "Provided JSON does not match rpc_message format";

                                socket_.async_write_some(
                                        boost::asio::buffer(invalid_message.to_json().dump()),
                                        [](boost::system::error_code ec, std::size_t sz) {});

                                continue;
                            }

                            auto response = dispatcher_->dispatch(message.procedure_name, message);
                            response.message_id = message.message_id;
                            response.source = source_;
                            response.destination = dst;

                            socket_.async_write_some(
                                    boost::asio::buffer(response.to_json().dump()),
                                    [](boost::system::error_code ec, std::size_t sz) {});
                        }

                        if (v.size() == 0) {
                            rpc_message invalid_message = construct_invalid_message();
                            invalid_message.source = source_;
                            invalid_message.destination = dst;
                            invalid_message.status_code = RpcStatusCode::InvalidRequest;
                            invalid_message.payload = "Provided message isn't valid JSON";

                            socket_.async_write_some(
                                    boost::asio::buffer(invalid_message.to_json().dump()),
                                    [](boost::system::error_code ec, std::size_t sz) {});
                        }

                        memset(temp_buffer_, 0, BUFFER_SIZE);
                        do_read();
                    } else {
                        if (error == boost::asio::error::eof) {
                            std::cout << "Connection terminated by peer." << std::endl;
                        } else {
                            std::cout << "Error during async read: " << error.message() << std::endl;
                        }
                    }
                }
        );
    }

}