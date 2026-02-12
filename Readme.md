# Multithreaded TCP Communication Framework (C++17, Linux)

A high-performance multithreaded TCP client-server framework built using:

- C++17
- POSIX sockets
- epoll (Linux I/O multiplexing)
- STL threading and synchronization
- gdb debugging
- valgrind performance & memory analysis

This project demonstrates scalable server design, concurrent client handling, and low-level Linux system programming.

---

## 🚀 Features

- Multi-client TCP server
- epoll-based scalable I/O handling
- Thread pool using std::thread
- Task queue with mutex + condition_variable
- Echo communication model
- Debug-ready (-g enabled)
- Memory and race detection ready
- Clean CMake build system

---

## 🏗 Architecture

Client → TCP → Server  
               ↓  
          epoll_wait()  
               ↓  
      Ready client socket  
               ↓  
         Task pushed to queue  
               ↓  
        Worker thread processes  

---

## 📂 Project Structure

tcp_framework/
│
├── include/
│ ├── ThreadPool.h
│ └── Server.h
│
├── src/
│ ├── main_server.cpp
│ ├── main_client.cpp
│ ├── ThreadPool.cpp
│ └── Server.cpp
│
├── CMakeLists.txt
└── README.md


---

## 🛠 Requirements

Ubuntu / Linux environment

Install dependencies:

```bash
sudo apt update
sudo apt install build-essential
sudo apt install gdb
sudo apt install valgrind
sudo apt install cmake
```


# Build Instructions

## Step 1: Create build directory
```bash
mkdir build
cd build
```

## Step 2: Generate build files
```bash
cmake ..
```

## Step 3: Compile
```bash
make
```

## Output:
build/server
build/client

# ▶️ Running the Application

## Terminal 1 – Start Server
```bash
./server
```


## Expected output:

Server started on port 8080

## Terminal 2 – Run Client
```bash
./client
```


## Expected output:

Server reply: Hello from client

# 🧠 Technical Design
## epoll

1. Efficient O(1) event notification

2. Scalable for thousands of file descriptors

3. Better than select() and poll()

## Thread Pool

1. Avoids overhead of creating threads per client

2. Fixed worker thread model

3. Tasks processed from queue

## Synchronization

1. std::mutex

2. std::condition_variable

3. std::atomic<bool>

# 🐞 Debugging with GDB

Compile in debug mode (already enabled in CMake).

## Start gdb:

```bash
gdb ./server
```


## Common commands:

```bash
break main
run
next
step
print variable
info threads
thread apply all bt
continue
quit
```

## Debugging Multithreading

```bash
info threads
thread 2
bt
```

# 🔍 Memory Analysis with Valgrind

## Memory Leak Detection
```bash
valgrind --leak-check=full ./server
```


## Look for:

```bash
definitely lost
indirectly lost
```

## Detect Race Conditions

```bash
valgrind --tool=helgrind ./server
```

## Performance Profiling

```bash
valgrind --tool=callgrind ./server
callgrind_annotate callgrind.out.*
```

# 📈 Performance Characteristics

1. O(1) epoll event detection

2. Controlled concurrency via thread pool

3. Minimal thread creation overhead

4. Suitable for high-connection environments