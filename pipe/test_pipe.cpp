#include <thread>

#include "non_blocking_pipe.hpp"

int main() {
    // Define the pipe name
    const std::string pipeName = "/tmp/my_pipe";

    // Create non-blocking publisher and subscriber
    NonBlockingPipe publisher(pipeName);
    NonBlockingPipe subscriber(pipeName);

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