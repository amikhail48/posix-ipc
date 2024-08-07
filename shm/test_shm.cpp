/**
 * @file test_shm.cpp
 * @author Andrew Mikhail
 * @brief This file tests the NonBlockingSHM Object
 * @version 0.1
 * @date 2024-08-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <iostream>

#include "non_blocking_shm.hpp"

int main() {
    const std::string shmName = "/test_shm";
    NonBlockingSHM writer(shmName);
    NonBlockingSHM reader(shmName);

    if (!writer.isOpen() || !reader.isOpen()) {
        std::cerr << "Failed to open shared memory." << std::endl;
        return 1;
    }

    std::string message = "Hello, Shared Memory!";
    writer.write(message);
    std::cout << "Wrote message: " << message << std::endl;

    std::string receivedMessage;
    reader.read(receivedMessage);
    std::cout << "Received message: " << receivedMessage << std::endl;

    int msg = 300;
    writer.write(300);
    std::cout << "Wrote message: " << msg << std::endl;

    int readMsg;
    reader.read(readMsg);
    std::cout << "Received message: " << readMsg << std::endl;

    return 0;
}
