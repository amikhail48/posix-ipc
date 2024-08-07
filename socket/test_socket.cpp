/**
 * @file test_socket.cpp
 * @author Andrew Mikhail
 * @brief This file tests the unix domain socket wrapper
 * @version 0.1
 * @date 2024-08-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <thread>

#include "non_blocking_unix_socket.hpp"

void server() {
    NonBlockingUnixSocket serverSocket(true);

    while (true) {
        std::string receivedMessage;
        if (serverSocket.read(receivedMessage)) {
            std::cout << "Server received message: " << receivedMessage << std::endl;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void client() {
    NonBlockingUnixSocket clientSocket(false);

    std::string message = "Hello, UNIX Domain Socket!";
    while (true) {
        if (clientSocket.write(message)) {
            std::cout << "Client sent message: " << message << std::endl;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    std::thread serverThread(server);
    std::thread clientThread(client);

    serverThread.join();
    clientThread.join();

    return 0;
}
