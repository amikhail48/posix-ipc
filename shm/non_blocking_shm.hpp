/**
 * @file non_blocking_shm.hpp
 * @author Andrew Mikhail 
 * @brief This file implements a C++ wrapper for POSIX Shared Memory
 * @version 0.1
 * @date 2024-08-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/* CPP Includes */
#include <cerrno>
#include <iostream>
#include <sstream>

/* C Includes */
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

/* Defines */
#define SHM_SIZE 4096

/**
 * @brief Non-blocking Message Queue Object
 * 
 */
class NonBlockingSHM {
public:
    /**
     * @brief Construct a new Non Blocking S H M object
     * 
     * @param name 
     */
    NonBlockingSHM(const std::string& name) : shmName_(name) {
        // Open shared memory object
        shmFd_ = shm_open(shmName_.c_str(), O_CREAT | O_RDWR, 0666);
        if (shmFd_ == -1) {
            perror("shm_open");
            exit(EXIT_FAILURE);
        }

        // Check size if already created
        if (shmSize() != SHM_SIZE)
        {
            // Set size of the shared memory object
            if (ftruncate(shmFd_, SHM_SIZE) == -1) {
                perror("ftruncate");
                exit(EXIT_FAILURE);
            }
        }

        // Map shared memory object
        shmPtr_ = mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd_, 0);
        if (shmPtr_ == MAP_FAILED) {
            perror("mmap");
            exit(EXIT_FAILURE);
        }
    }

    /**
     * @brief Destroy the Non Blocking S H M object
     * 
     */
    ~NonBlockingSHM() {
        if (shmPtr_ != MAP_FAILED) {
            munmap(shmPtr_, SHM_SIZE);
        }
        if (shmFd_ != -1) {
            close(shmFd_);
        }
        shm_unlink(shmName_.c_str());
    }

    /**
     * @brief Check if shared memory block is usable
     * 
     * @return true 
     * @return false 
     */
    bool isOpen() const {
        return shmPtr_ != MAP_FAILED && shmFd_ != -1;
    }

    /**
     * @brief Write data onto the shared memory
     * 
     * @tparam T 
     * @param data 
     * @return true 
     * @return false 
     */
    template<typename T>
    bool write(const T& data) {
        if (!isOpen()) return false;

        std::string message = to_string(data);
        if (message.size() + 1 > SHM_SIZE) { // +1 for null terminator
            std::cerr << "Message size exceeds shared memory size." << std::endl;
            return false;
        }

        std::memset(shmPtr_, 0, SHM_SIZE); // Clear the shared memory before writing
        std::memcpy(shmPtr_, message.c_str(), message.size() + 1);

        return true;
    }

    /**
     * @brief Read data from the shared memory
     * 
     * @tparam T 
     * @param data 
     * @return true 
     * @return false 
     */
    template<typename T>
    bool read(T& data) {
        if (!isOpen()) return false;

        std::string message(static_cast<char*>(shmPtr_));

        data = from_string<T>(message);
        return true;
    }

private:
    std::string shmName_;
    int shmFd_;
    void* shmPtr_;

    /**
     * @brief Fetch shared memory block size
     * 
     * @return ssize_t 
     */
    ssize_t shmSize() {
        struct stat sb;
        if (fstat(shmFd_, &sb) == -1) {
            perror("fstat");
            close(shmFd_);
            exit(EXIT_FAILURE);
        }
        
        return sb.st_size;
    }
    
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
