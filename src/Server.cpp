#include "Server.h"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstring>

#define MAX_EVENTS 10
#define BUFFER_SIZE 1024

Server::Server(int port, int threads)
    : port(port), pool(threads) {}

void Server::setupSocket() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, SOMAXCONN);

    epoll_fd = epoll_create1(0);

    epoll_event event{};
    event.events = EPOLLIN;
    event.data.fd = server_fd;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);
}

void Server::handleClient(int client_fd) {
    char buffer[BUFFER_SIZE];
    int bytes = read(client_fd, buffer, sizeof(buffer));

    if (bytes > 0) {
        std::string msg(buffer, bytes);
        std::cout << "Received: " << msg << std::endl;
        write(client_fd, msg.c_str(), msg.size());
    } else {
        close(client_fd);
    }
}

void Server::start() {
    setupSocket();
    std::cout << "Server started on port " << port << std::endl;

    epoll_event events[MAX_EVENTS];

    while (true) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < nfds; ++i) {

            if (events[i].data.fd == server_fd) {
                int client_fd = accept(server_fd, nullptr, nullptr);

                epoll_event event{};
                event.events = EPOLLIN;
                event.data.fd = client_fd;

                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);

            } else {
                int client_fd = events[i].data.fd;

                pool.enqueue([this, client_fd]() {
                    handleClient(client_fd);
                });
            }
        }
    }
}
