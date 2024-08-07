/**
 * @file test_mqueue.cpp
 * @author Andrew Mikhail
 * @brief  This file tests the NonBlockingMQ Object
 * @version 0.1
 * @date 2024-08-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "non_blocking_mqueue.hpp"

int main() {
    // Define the pipe name
    const std::string queueName = "my_mq";

    // Create non-blocking publisher and subscriber
    NonBlockingMQ publisher(queueName);
    NonBlockingMQ subscriber(queueName);

    if (publisher.isOpen()) {
        std::cout << "Publisher is open." << std::endl;
    }

    int dataToPublish = 42;
    if (publisher.write(dataToPublish)) {
        std::cout << "Published data: " << dataToPublish << std::endl;
    }

    if (subscriber.isOpen()) {
        std::cout << "Subsriber is open." << std::endl;
    }
    int receivedData;
    if (subscriber.read(receivedData)) {
        std::cout << "Received data: " << receivedData << std::endl;
    } else {
        std::cerr << "No data received." << std::endl;
    }

    return 0;
}