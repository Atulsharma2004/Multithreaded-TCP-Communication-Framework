#include <iostream>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <atomic>
#include <mutex>

#define PORT 8080
#define BUFFER_SIZE 4096

std::atomic<bool> running(true);
std::mutex coutMutex;

void receiveMessages(int sock) {
    char buffer[BUFFER_SIZE];

    while (running) {
        int bytes = read(sock, buffer, sizeof(buffer) - 1);

        if (bytes > 0) {
            buffer[bytes] = '\0';

            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "\nServer: " << buffer << std::endl;
        }
        else if (bytes == 0) {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "\nServer closed connection.\n";
            running = false;
            break;
        }
        else {
            running = false;
            break;
        }
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        return 1;
    }

    std::cout << "Connected to server.\n";

    std::thread receiver(receiveMessages, sock);

    std::string message;

    while (running) {
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "You: ";
        }

        std::getline(std::cin, message);

        if (message == "exit") {
            running = false;
            break;
        }

        send(sock, message.c_str(), message.size(), 0);
    }

    close(sock);

    if (receiver.joinable())
        receiver.join();

    std::cout << "Connection closed.\n";

    return 0;
}
