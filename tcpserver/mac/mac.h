#ifndef MACSERVER_H
#define MACSERVER_H

#include "./tcpserver.h"
#include <iostream>
#include <thread>
#include <unordered_map>

class MacServer : public TCPServer {
private:
    int server_fd;
    int kqueue_fd;
    std::thread kqueueThread;

    void kqueueLoop();
    bool setSocketNonBlocking(int sockfd);

    void handleRecv(int client_socket);
    void handleSend(int client_socket);
    void handleError(int client_socket);

    std::unordered_map<int, std::string> sendBuffers;

public:
    MacServer();
    bool initialize(int port, const std::string &ipAddress = "127.0.0.1") override;
    void start() override;
    virtual ~MacServer();
};

#endif
