import socket
import time
import json
import random

# Remote host and port
host = '127.0.0.1'
port = 8000

# Message to send
data = {
    "source": {
        "address": "0.0.0.0",
        "port": 8001
    },
    "destination": {
        "address": "127.0.0.1",
        "port": 60322
    },
    "headers": {},
    "message_id": "0",
    "procedure_name": "add",
    "status_code": 1
}

#Create a socket object
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((host, port))

counter = 0

start_time = time.time()
end_time = start_time + 10

while time.time() < end_time:
    data["payload"] = str(counter)
    counter += 1
    message = json.dumps(data)
    sock.sendall(message.encode())
    response = sock.recv(1)
    # d = json.loads(response.decode())

print(counter)


