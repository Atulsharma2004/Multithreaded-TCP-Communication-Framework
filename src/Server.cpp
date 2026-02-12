#include "Server.h"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstring>
#include <errno.h>

#define MAX_EVENTS 64
#define BUFFER_SIZE 4096

Server::Server(int port, int threads)
    : port(port), running(true), pool(threads) {}

Server::~Server() {
    stop();
}

void Server::setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void Server::setupSocket() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, SOMAXCONN);

    setNonBlocking(server_fd);

    epoll_fd = epoll_create1(0);

    epoll_event event{};
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = server_fd;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);

    std::cout << "Server started on port " << port << std::endl;
}

void Server::acceptClients() {
    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);

        if (client_fd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            else
                perror("accept");
        }

        setNonBlocking(client_fd);

        epoll_event event{};
        event.events = EPOLLIN | EPOLLET;
        event.data.fd = client_fd;

        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);

        client_fds.insert(client_fd);
        std::cout << "New client connected: " << client_fd << std::endl;
    }
}

void Server::handleClient(int client_fd) {
    char buffer[BUFFER_SIZE];

    while (true) {
        int bytes = read(client_fd, buffer, sizeof(buffer));

        if (bytes > 0) {
            write(client_fd, buffer, bytes);
        }
        else if (bytes == 0) {
            std::cout << "Client disconnected: " << client_fd << std::endl;
            close(client_fd);
            client_fds.erase(client_fd);
            break;
        }
        else {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            else {
                perror("read");
                close(client_fd);
                client_fds.erase(client_fd);
                break;
            }
        }
    }
}

void Server::start() {
    setupSocket();

    epoll_event events[MAX_EVENTS];

    while (running) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < nfds; ++i) {

            if (events[i].data.fd == server_fd) {
                acceptClients();
            } else {
                int client_fd = events[i].data.fd;

                pool.enqueue([this, client_fd]() {
                    handleClient(client_fd);
                });
            }
        }
    }
}

void Server::stop() {
    running = false;

    for (int fd : client_fds) {
        close(fd);
    }

    if (server_fd > 0)
        close(server_fd);

    if (epoll_fd > 0)
        close(epoll_fd);

    std::cout << "Server shutdown complete." << std::endl;
}
