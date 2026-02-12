#ifndef SERVER_H
#define SERVER_H

#include <atomic>
#include <unordered_set>
#include "ThreadPool.h"

class Server {
public:
    Server(int port, int threads);
    ~Server();

    void start();
    void stop();

private:
    int port;
    int server_fd;
    int epoll_fd;

    std::atomic<bool> running;

    ThreadPool pool;
    std::unordered_set<int> client_fds;

    void setupSocket();
    void setNonBlocking(int fd);
    void handleClient(int client_fd);
    void acceptClients();
};

#endif
