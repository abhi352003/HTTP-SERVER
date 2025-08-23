#include "lin.h"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

LinClient::LinClient() : sockfd(-1) {}

LinClient::~LinClient() {
    disconnect();
}

bool LinClient::connectToServer(const std::string &ip, int port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed\n";
        return false;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported\n";
        return false;
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return false;
    }

    std::cout << "Connected to server " << ip << ":" << port << "\n";
    return true;
}

bool LinClient::sendData(const std::string &data) {
    if (sockfd < 0) return false;
    return send(sockfd, data.c_str(), data.size(), 0) >= 0;
}

std::string LinClient::receiveData() {
    if (sockfd < 0) return "";
    char buffer[1024] = {0};
    int valread = recv(sockfd, buffer, sizeof(buffer), 0);
    return (valread > 0) ? std::string(buffer, valread) : "";
}

void LinClient::disconnect() {
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1;
        std::cout << "Disconnected from server\n";
    }
}
