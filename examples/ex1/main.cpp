#include "simple-rpc/rpc_client.h"
#include "simple-rpc/client.h"
#include "simple-rpc/server.h"

#include <iostream>
#include <thread>
#include <future>

int main() {
    std::thread server_thread([]() {
        srpc::server s{"0.0.0.0", 8000};
        s.run();
    });

    srpc::rpc_message message;
    message.status_code = srpc::RpcErrorCode::Placeholder;
    message.procedure_name = "example_procedure";
    message.error_message = "";
    message.payload = "Some payload";
    message.headers["Content-Type"] = "application/json";
    message.headers["Authorization"] = "Bearer abc123";

    std::thread client_thread([&message]() {
        srpc::rpc_client c;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        c.connect({"0.0.0.0", 8000});

        for (int i=0; i<10; i++) {
            std::future<srpc::rpc_message> fut = c.send(message, [](boost::system::error_code error, std::size_t bytes) {});
            std::cout << fut.get().to_json() << std::endl;
        }
    });

    server_thread.join();
    client_thread.join();
}