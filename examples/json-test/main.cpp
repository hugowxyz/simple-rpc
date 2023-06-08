#include "nlohmann/json.h"
#include "simple-rpc/utils.h"
#include "simple-rpc/rpc_message.h"

#include <iostream>

using json = nlohmann::json;

int main() {
    rpc_message message;
    message.message_id = 1;
    message.source = {"127.0.0.1", 8080};
    message.destination = {"192.168.0.1", 8000};
    message.status_code = RpcErrorCode::Placeholder;
    message.procedure_name = "example";
    message.error_message = "";
    message.payload = "data";
    message.headers = {{"Header1", "Value1"}, {"Header2", "Value2"}};

    conn_info info{"12345", 1000};

    json j = message.to_json();
    rpc_message message1 = rpc_message::from_json(j);

    std::cout << message1.headers["Header1"] << std::endl;
}