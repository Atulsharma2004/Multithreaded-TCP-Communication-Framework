#ifndef SERVER_H
#define SERVER_H

#include <string>
#include "ThreadPool.h"

class Server {
public:
    Server(int port, int threads);
    void start();

private:
    int port;
    int server_fd;
    int epoll_fd;
    ThreadPool pool;

    void setupSocket();
    void handleClient(int client_fd);
};

#endif

