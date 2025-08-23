#include "win.h"
#include <iostream>

WinClient::WinClient() : sockfd(INVALID_SOCKET) {
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
    }
}

WinClient::~WinClient() {
    disconnect();
    WSACleanup();
}

bool WinClient::connectToServer(const std::string &ip, int port) {
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        return false;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr);

    if (connect(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed\n";
        closesocket(sockfd);
        return false;
    }

    std::cout << "Connected to server " << ip << ":" << port << "\n";
    return true;
}

bool WinClient::sendData(const std::string &data) {
    return send(sockfd, data.c_str(), (int)data.size(), 0) != SOCKET_ERROR;
}

std::string WinClient::receiveData() {
    char buffer[1024] = {0};
    int valread = recv(sockfd, buffer, sizeof(buffer), 0);
    return (valread > 0) ? std::string(buffer, valread) : "";
}

void WinClient::disconnect() {
    if (sockfd != INVALID_SOCKET) {
        closesocket(sockfd);
        sockfd = INVALID_SOCKET;
        std::cout << "Disconnected from server\n";
    }
<<<<<<< HEAD
}
=======
}
>>>>>>> 6b9f5c30c30e3216c705669f58b91ab5b49477d5
