#ifndef WINSERVER_H
#define WINSERVER_H

#include "./tcpserver.h"
#include <iostream>
#include <thread>
#include <unordered_map>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

class WinServer : public TCPServer {
private:
    SOCKET server_fd;
    std::thread pollThread;
    std::vector<WSAPOLLFD> pollFds;
    std::unordered_map<SOCKET, std::string> sendBuffers;

    void pollLoop();

    void handleRecv(SOCKET client_socket);
    void handleSend(SOCKET client_socket);
    void handleError(SOCKET client_socket);

public:
    WinServer();
    bool initialize(int port, const std::string &ipAddress = "127.0.0.1") override;
    void start() override;
    virtual ~WinServer();
};

#endif
