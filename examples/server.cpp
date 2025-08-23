#include "tcpserver.h"
#include <iostream>
#include <string>

int main() {
    // Create server instance using factory
    TCPServer* server = createserver();

    if (!server->initialize(8080, "127.0.0.1")) {
        std::cerr << "Server initialization failed!" << std::endl;
        return -1;
    }

    std::cout << "Server started on 127.0.0.1:8080" << std::endl;
    server->start();

    delete server;
    return 0;
}