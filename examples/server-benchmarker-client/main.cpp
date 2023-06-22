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
    std::vector<std::thread> threads;
    for (int i=0; i<6; i++) {
        threads.emplace_back([]() {
            client c;
            int totalRequests = 10000;
            conn_info remote = {"0.0.0.0", 8000};
            rpc_message message(RpcStatusCode::Placeholder, "1");
            message.procedure_name = "add";

            auto start = std::chrono::steady_clock::now();
            for (int i=0; i < totalRequests; i++) {
//                message.payload = std::to_string(i);
                auto fut = c.send(remote, message);
                fut.get();
            }
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