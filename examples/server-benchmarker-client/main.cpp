#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "nlohmann/json.h"
using json = nlohmann::json;

#include "simple-rpc/separate_json.h"
#include "simple-rpc/client.h"

using namespace srpc;

int main(int argc, char* argv[]) {
//    const auto processor_count = std::thread::hardware_concurrency();
    const auto processor_count = 2;
    std::vector<std::thread> threads;
    for (int i=0; i<processor_count; i++) {
        threads.emplace_back([]() {
            client c;
            int totalRequests = 1000;
            conn_info remote = {"0.0.0.0", 8000};
            rpc_message message(RpcStatusCode::Placeholder, "1");
            message.procedure_name = "add";

            std::vector<std::future<rpc_message>> futures;

            auto start = std::chrono::steady_clock::now();
            for (int i=0; i < totalRequests; i++) {
                message.payload = std::to_string(i);
                auto fut = c.send(remote, message);
                futures.push_back(std::move(fut));
//                fut.get();
            }

            for (auto& f : futures) {
                f.get();
            }

//            while (futures.size() > 0) {
//                std::cout << futures.size() << std::endl;
//                futures.erase(std::remove_if(
//                        futures.begin(), futures.end(),
//                        [](auto& f) {
//                            return f.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
//                        }));
//            }

            auto end = std::chrono::steady_clock::now();
            double elapsedSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
            double throughput = totalRequests / elapsedSeconds;
            std::cout << "Total requests: " << totalRequests << std::endl;
            std::cout << "Elapsed time: " << elapsedSeconds << " seconds" << std::endl;
            std::cout << "Throughput: " << throughput << " requests/second" << std::endl;
        });
    }

    for (auto &t : threads) {
        t.join();
    }

    return 0;
}