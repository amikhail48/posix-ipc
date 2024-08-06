/**
 * @file non_blocking_pipe.hpp
 * @author Andrew Mikhail
 * @brief This file implements a C++ wrapper for POSIX Pipes
 * @version 0.1
 * @date 2024-08-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/* CPP Includes */
#include <iostream>
#include <sstream>
/* C Includes */
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

/* Defines */
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
 * @brief Non-blocking pipe wrapper object
 * 
 */
class NonBlockingPipe {
public:
    /**
     * @brief Construct a new Non Blocking Pipe object
     * 
     * @param pipeName 
     */
    NonBlockingPipe(const std::string &pipeName) : pipeName_(pipeName), fd_(-1) {
        if (pipeExists()) {
            openPipe();
        } else {
            createPipe();
        }
    }

    /**
     * @brief Destroy the Non Blocking Pipe object
     * 
     */
    ~NonBlockingPipe() {
        closePipe();
    }

    /**
     * @brief Write data onto the pipe
     * 
     * @tparam T 
     * @param data 
     * @return true 
     * @return false 
     */
    template <typename T>
    bool write(const T &data) {
        if (fd_ != -1) {
            std::string strData = to_string(data);
            ssize_t bytesWritten = ::write(fd_, strData.c_str(), strData.size());
            return bytesWritten == static_cast<ssize_t>(strData.size());
        }
        return false;
    }

    /**
     * @brief Read data from the pipe
     * 
     * @tparam T 
     * @param data 
     * @return true 
     * @return false 
     */
    template <typename T>
    bool read(T &data) {
        if (fd_ != -1) {
            char buffer[MAX_MSG_SIZE_BYTES]; /* Adjust the buffer size as needed */
            ssize_t bytesRead = ::read(fd_, buffer, sizeof(buffer));
            if (bytesRead > 0) {
                std::string strData(buffer, bytesRead);
                data = from_string<T>(strData);
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Check if pipe is open
     * 
     * @return true 
     * @return false 
     */
    bool isOpen() const {
        return fd_ != -1;
    }

private:
    std::string pipeName_;
    int fd_;

    /**
     * @brief Check if pipe exists
     * 
     * @return true 
     * @return false 
     */
    bool pipeExists() const {
        struct stat st;
        return stat(pipeName_.c_str(), &st) == 0;
    }

    /**
     * @brief Open the pipe
     * 
     */
    void openPipe() {
        fd_ = ::open(pipeName_.c_str(), O_RDWR | O_NONBLOCK);
    }

    /**
     * @brief Create a Pipe
     * 
     */
    void createPipe() {
        /* Create a non-blocking named pipe */
        if (mkfifo(pipeName_.c_str(), 0666) == 0) {
            openPipe();
        }
    }

    /**
     * @brief Close the pipe
     * 
     */
    void closePipe() {
        if (fd_ != -1) {
            close(fd_);
            unlink(pipeName_.c_str());
        }
    }
};
