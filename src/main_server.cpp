#include "Server.h"
#include <signal.h>
#include <memory>

std::unique_ptr<Server> server;

void signalHandler(int signum) {
    if (server) {
        server->stop();
    }
    exit(signum);
}

int main() {
    signal(SIGINT, signalHandler);

    server = std::make_unique<Server>(8080, 4);
    server->start();

    return 0;
}
