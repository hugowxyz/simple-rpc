#include "simple-rpc/client.h"
#include "simple-rpc/server.h"

#include <iostream>
#include <thread>
#include <future>
#include <vector>
#include <string>
#include <random>

int main() {
    std::thread server_thread([]() {
        srpc::server s{"0.0.0.0", 8000};

        s.bind("add", [](const rpc_message& message) {
            int a, b;
            try {
                json payload = json::parse(message.payload);
                a = payload["a"].get<int>();
                b = payload["b"].get<int>();
            } catch (const std::exception& e) {
                std::cout << e.what() << std::endl;
                rpc_message reply;
                reply.status_code = RpcStatusCode::InvalidRequest;
                reply.payload = "Invalid JSON provided";

                return reply;
            }

            auto result = a + b;
            rpc_message reply = message;
            reply.status_code = RpcStatusCode::Success;
            reply.payload = std::to_string(result);
            return reply;
        });

        s.run();
    });

    rpc_message message;
    message.status_code = RpcStatusCode::Placeholder;
    message.procedure_name = "add";

    std::thread client_thread([&message]() {
        srpc::client c;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        c.connect({"0.0.0.0", 8000});

        std::vector<std::future<rpc_message>> futs;

        std::random_device rd;
        std::mt19937 gen(rd());

        // Define the range of random numbers (inclusive)
        int lowerBound = 1;
        int upperBound = 100;

        // Create a uniform distribution for random numbers within the range
        std::uniform_int_distribution<int> dist(lowerBound, upperBound);

        for (int i=0; i<10; i++) {
            json args = {
                    {"a", dist(gen)},
                    {"b", dist(gen)}
            };
            message.payload = args.dump();

            std::future<rpc_message> fut = c.send(message);
            futs.push_back(std::move(fut));
        }

        for (auto& fut : futs) {
            std::cout << "Response: " << fut.get().to_json() << std::endl;
        }
    });

    server_thread.join();
    client_thread.join();
}