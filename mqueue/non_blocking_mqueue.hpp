/**
 * @file non_blocking_mqueue.hpp
 * @author Andrew Mikhail
 * @brief This file implements a C++ wrapper for POSIX Message Queues
 * @version 0.1
 * @date 2024-08-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/* CPP Includes */
#include <iostream>
#include <string>

/* C Includes */
#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <unistd.h>

/* Defines */
#define MAX_NUM_MSGS       10
#define MAX_MSG_SIZE_BYTES 4096

/**
 * @brief Helper function to convert any data type to a string
 * 
 * @tparam T 
 * @param value 
 * @return std::string 
 */
template <typename T>
std::string to_string(const T &value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

/**
 * @brief Helper function to convert a string to any data type
 * 
 * @tparam T 
 * @param str 
 * @return T 
 */
template <typename T>
T from_string(const std::string &str) {
    T value;
    std::istringstream(str) >> value;
    return value;
}

/**
 * @brief Non-blocking Message Queue Object
 * 
 */
class NonBlockingMQ {
public:
    /**
     * @brief Construct a new Non Blocking MQ object
     * 
     * @param queueName 
     */
    NonBlockingMQ(const std::string& queueName) : queueName_(queueName) {
        mq_unlink(queueName.c_str());  // Ensure the queue doesn't exist
        struct mq_attr attr = {};
        attr.mq_flags = 0;
        attr.mq_maxmsg = MAX_NUM_MSGS;  // Max number of messages
        attr.mq_msgsize = MAX_MSG_SIZE_BYTES; // Max size of each message

        mq_ = mq_open(queueName.c_str(), O_CREAT | O_RDWR | O_NONBLOCK, 0644, &attr);
        if (mq_ == -1) {
            perror("mq_open");
            exit(EXIT_FAILURE);
        }
    }

    /**
     * @brief Destroy the Non Blocking M Q object
     * 
     */
    ~NonBlockingMQ() {
        mq_close(mq);
    }

    /**
     * @brief Write data onto the message queue
     * 
     * @tparam T 
     * @param data 
     * @return true 
     * @return false 
     */
    template<typename T>
    bool write(const T& data) {
        bool status = true;
        std::string message = to_string(data);
        if (mq_send(mq, message.c_str(), message.size() + 1, 0) == -1) {
            status = false;
            if (errno == EAGAIN) {
                std::cerr << "Queue is full, message not sent." << std::endl;
            } else {
                perror("mq_send");
            }
        }

        return status;
    }

    /**
     * @brief Read data from the message queue
     * 
     * @tparam T 
     * @param data 
     * @return true 
     * @return false 
     */
    template<typename T>
    bool read(T& data) {
        bool status = true;
        char buffer[MAX_MSG_SIZE_BYTES];
        ssize_t bytesRead = mq_receive(mq, buffer, sizeof(buffer), nullptr);
        if (bytesRead == -1) {
            status = false;
            if (errno == EAGAIN) {
                std::cerr << "Queue is empty, no message received.\n";
            } else {
                perror("mq_receive");
            }
        }

        std::string message(buffer, bytesRead);
        data = from_string<T>(message);
        return status;
    }

    /**
     * @brief Check if message queue is open
     * 
     * @return true 
     * @return false 
     */
    bool isOpen() const {
        return mq_ != (mqd_t)-1;
    }

private:
    std::string queueName_;
    mqd_t mq_;
};
