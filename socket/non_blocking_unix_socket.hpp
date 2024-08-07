/**
 * @file non_blocking_unix_socket.hpp
 * @brief This file implements a C++ wrapper for UNIX domain sockets
 * @version 0.1
 * @date 2024-08-07
 * 
 */

#ifndef NON_BLOCKING_UNIX_SOCKET_HPP
#define NON_BLOCKING_UNIX_SOCKET_HPP

/* CPP Includes */
#include <iostream>
#include <sstream>

/* C Includes */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

/* Defines */
#define SOCKET_PATH "non_blocking_socket"
#define BUFFER_SIZE 4096

/**
 * @brief Non blocking Unix Domain Socket Object
 * 
 */
class NonBlockingUnixSocket {
public:
    /**
     * @brief Construct a new Non Blocking Unix Socket object
     * 
     * @param isServer 
     */
    NonBlockingUnixSocket(bool isServer) {
        isServer_ = isServer;

        /* Create socket */
        socketFd_ = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (socketFd_ == -1) {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        /* Set socket to non-blocking mode */
        int flags = fcntl(socketFd_, F_GETFL, 0);
        fcntl(socketFd_, F_SETFL, flags | O_NONBLOCK);

        /* Initialize socket address */
        memset(&addr_, 0, sizeof(addr_));
        addr_.sun_family = AF_UNIX;
        strncpy(addr_.sun_path, SOCKET_PATH, sizeof(addr_.sun_path) - 1);

        if (isServer_) {
            /* Bind the socket */
            unlink(SOCKET_PATH);
            if (bind(socketFd_, (struct sockaddr *)&addr_, sizeof(addr_)) == -1) {
                perror("bind");
                exit(EXIT_FAILURE);
            }
        }
    }

    /**
     * @brief Destroy the Non Blocking Unix Socket object
     * 
     */
    ~NonBlockingUnixSocket() {
        close(socketFd_);
        if (isServer_) {
            unlink(SOCKET_PATH);
        }
    }

    /**
     * @brief Write data onto the socket
     * 
     * @tparam T 
     * @param data 
     * @return true 
     * @return false 
     */
    template<typename T>
    bool write(const T& data) {
        if (!serverExists()) {
            std::cerr << "Server socket does not exist yet.\n";
            return false;
        }

        std::string message = to_string(data);
        ssize_t bytesSent = sendto(socketFd_, message.c_str(), message.size(), 0, (struct sockaddr *)&addr_, sizeof(addr_));
        if (bytesSent == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::cerr << "Socket is not ready for writing, try again later.\n";
            } else {
                perror("sendto");
            }
            return false;
        }
        return true;
    }

    /**
     * @brief Read data from the socket
     * 
     * @tparam T 
     * @param data 
     * @return true 
     * @return false 
     */
    template<typename T>
    bool read(T& data) {
        char buffer[BUFFER_SIZE];
        socklen_t addrLen = sizeof(addr_);
        ssize_t bytesRead = recvfrom(socketFd_, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr_, &addrLen);
        if (bytesRead == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::cerr << "No data available to read.\n";
            } else {
                perror("recvfrom");
            }
            return false;
        }

        std::string message(buffer, bytesRead);
        data = from_string<T>(message);
        return true;
    }

    /**
     * @brief Check is server exists
     * 
     * @return true 
     * @return false 
     */
    bool serverExists() {
        struct stat buffer;
        return (stat(SOCKET_PATH, &buffer) == 0);
    }

private:
    int socketFd_;
    struct sockaddr_un addr_;
    bool isServer_;

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
    typename std::enable_if<!std::is_same<T, std::string>::value, T>::type
    from_string(const std::string &str) {
        T value;
        std::istringstream(str) >> value;
        return value;
    }

    /**
     * @brief Helper function to return string read without modifying
     * 
     * @tparam T 
     * @param str 
     * @return std::enable_if<std::is_same<T, std::string>::value, T>::type 
     */
    template <typename T>
    typename std::enable_if<std::is_same<T, std::string>::value, T>::type
    from_string(const std::string &str) {
        return str;
    }
};

#endif /* NON_BLOCKING_UNIX_SOCKET_HPP */
