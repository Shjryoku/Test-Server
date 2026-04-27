# Test Server

A lightweight, high-performance TCP/UDP test server written in C++ using epoll for asynchronous I/O on Linux.

## Overview

This is a simple but efficient test server designed for network testing, client debugging, and development experiments.

### Main Features

- Supports both **TCP** and **UDP** (switchable via flag)
- Uses **epoll** for efficient event-driven I/O (non-blocking)
- Built-in simple command system:
  - `/time` — returns current server time
  - `/stats` — shows total and current connected clients
  - `/shutdown` — gracefully stops the server
- Echo mode for any other messages (sends received data back to client)
- Real-time logging of client connections/disconnections
- Clean object-oriented design with separate socket classes
- Ready for production-like deployment:
  - Debian packaging support (`debian/`)
  - Systemd service support (`systemd/`)

## Technologies

- **C++17**
- **C** (low-level socket parts)
- **epoll** (Linux event notification)
- **CMake** + Make (build system)
- Non-blocking sockets
- Systemd & Debian packaging

## How It Works

The server listens on `127.0.0.1:8080` by default (can be changed in `main()`).

**Available commands** (send as text starting with `/`):

- `/time` → Returns current date and time
- `/stats` → Shows connection statistics
- `/shutdown` → Stops the server
- Any other text → Echoed back to the client

## Building the Project

### Prerequisites

- CMake 3.10+
- GCC/G++ (or Clang)
- Linux environment (epoll is Linux-specific)

### Build Instructions

```bash
git clone https://github.com/Shjryoku/Test-Server.git
cd Test-Server

mkdir -p build && cd build

cmake ..
make -j$(nproc)
```

## Running the Server
```bash
cd build
./server
```

## By default the server starts on 127.0.0.1:8080 in TCP mode.
To run in UDP mode, change the line bool useUDP = false; to true in main() and rebuild.

## Available Commands

When connected to the server, you can send the following commands (starting with /):

```bash /time``` — Get current server time
```bash /stats``` — Show connection statistics
```bash /shutdown``` — Stop the server
Any other text — Echoed back to the client

## Example with netcat
```bash
nc 127.0.0.1 8080
```

Then type:
```bash
/time
/stats
/shutdown
```

## Note:
This is an old personal project. The code is functional, but documentation was minimal. Feel free to improve the project or add new features.
text
