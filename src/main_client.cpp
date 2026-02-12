#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define PORT 8080

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    std::string message = "Hello from client";
    send(sock, message.c_str(), message.size(), 0);

    char buffer[1024];
    int bytes = read(sock, buffer, 1024);

    std::cout << "Server reply: " << std::string(buffer, bytes) << std::endl;

    close(sock);
    return 0;
}
