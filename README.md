# linux-utils

# IPC Wrappers for POSIX Message Queues, Shared Memory, and Pipes

This repository contains C++ wrappers for POSIX Message Queues, Shared Memory, and Pipes, providing a convenient interface for inter-process communication (IPC) in non-blocking mode. The implementation includes support for generic data types using C++ templates.

## Overview

### POSIX Message Queues

The `NonBlockingMQ` class provides a non-blocking interface for POSIX Message Queues, allowing for sending and receiving messages of any data type.

### POSIX Shared Memory

The `NonBlockingSHM` class provides a non-blocking interface for POSIX Shared Memory, enabling processes to share data efficiently.

### Pipes

The `NonBlockingPipe` class provides a non-blocking interface for named pipes (FIFO), supporting generic data transport.

## Prerequisites

- C++ compiler supporting C++11 or later
- POSIX-compliant operating system (Linux, macOS, etc.)
- Bazel build system

## Getting Started

### Building the Project

Ensure Bazel is installed on your system. You can download and install Bazel from the [Bazel website](https://bazel.build/).

### Project Structure

.
├── LICENSE
├── README.md
├── WORKSPACE
├── mqueue
│   ├── BUILD
│   ├── non_blocking_mqueue.hpp
│   └── test_mqueue.cpp
├── pipe
│   ├── BUILD
│   ├── non_blocking_pipe.hpp
│   └── test_pipe.cpp
└── shm
    ├── BUILD
    ├── non_blocking_shm.hpp
    └── test_shm.cpp

## Usage

### Building and Running the tests
```bash
    bazel run mqueue:non_blocking_mqueue
```
```bash
    bazel run pipe:non_blocking_pipe
```
```bash
    bazel run shm:non_blocking_shm
```