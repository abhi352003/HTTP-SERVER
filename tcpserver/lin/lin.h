#ifndef LINSERVER_H
#define LINSERVER_H

#include "./tcpserver.h"
#include <iostream>
#include <thread>
#include <unordered_map>

class LinServer : public TCPServer {
private:
    int server_fd;
    int epoll_fd;
    std::thread epollThread;

    void epollLoop();
    bool setSocketNonBlocking(int sockfd);

    void handleRecv(int client_socket);
    void handleSend(int client_socket);
    void handleError(int client_socket);
    std::unordered_map<int, std::string> sendBuffers;

public:
    LinServer();
    bool initialize(int port, const std::string &ipAddress = "127.0.0.1") override;
    void start() override;
    virtual ~LinServer();
};

#endif
