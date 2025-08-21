#include "win.h"
#include <iostream>
#include <cstring>

using namespace std;

WinServer::WinServer() : server_fd(INVALID_SOCKET) {}

bool WinServer::initialize(int port, const std::string &ipAddress) {
    cout << "Initializing Windows server..." << endl;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed: " << WSAGetLastError() << endl;
        return false;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == INVALID_SOCKET) {
        cerr << "Socket creation failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return false;
    }

    BOOL opt = TRUE;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) == SOCKET_ERROR) {
        cerr << "setsockopt failed: " << WSAGetLastError() << endl;
        closesocket(server_fd);
        WSACleanup();
        return false;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (ipAddress == "0.0.0.0") {
        address.sin_addr.s_addr = INADDR_ANY;
    } else {
        if (InetPton(AF_INET, ipAddress.c_str(), &address.sin_addr) <= 0) {
            cerr << "Invalid IP address: " << ipAddress << endl;
            closesocket(server_fd);
            WSACleanup();
            return false;
        }
    }

    if (bind(server_fd, (sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        cerr << "Bind failed: " << WSAGetLastError() << endl;
        closesocket(server_fd);
        WSACleanup();
        return false;
    }

    if (listen(server_fd, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Listen failed: " << WSAGetLastError() << endl;
        closesocket(server_fd);
        WSACleanup();
        return false;
    }

    WSAPOLLFD serverPollFd;
    serverPollFd.fd = server_fd;
    serverPollFd.events = POLLRDNORM; // ready for accept
    serverPollFd.revents = 0;
    pollFds.push_back(serverPollFd);

    cout << "Windows server initialized successfully on " << ipAddress << ":" << port << endl;
    return true;
}

void WinServer::handleRecv(SOCKET client_fd) {
    char buffer[1024];
    int byteRead = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    if (byteRead > 0) {
        buffer[byteRead] = '\0';
        cout << "Received (" << byteRead << " bytes) from fd " << client_fd << ": " << buffer << endl;

        sendBuffers[client_fd] += buffer;
    } else if (byteRead == 0) {
        cout << "Client disconnected (fd " << client_fd << ")" << endl;
        closesocket(client_fd);
        sendBuffers.erase(client_fd);
    } else {
        cerr << "Recv failed: " << WSAGetLastError() << endl;
        closesocket(client_fd);
        sendBuffers.erase(client_fd);
    }
}

void WinServer::handleSend(SOCKET client_fd) {
    auto it = sendBuffers.find(client_fd);
    if (it == sendBuffers.end() || it->second.empty()) return;

    string &msg = it->second;
    int bytesSent = send(client_fd, msg.c_str(), (int)msg.size(), 0);

    if (bytesSent > 0) {
        msg.erase(0, bytesSent);
        cout << "Sent " << bytesSent << " bytes to fd " << client_fd << endl;
    } else {
        cerr << "Send failed: " << WSAGetLastError() << endl;
        closesocket(client_fd);
        sendBuffers.erase(client_fd);
    }
}

void WinServer::handleError(SOCKET client_fd) {
    cerr << "Socket error on fd " << client_fd << endl;
    closesocket(client_fd);
    sendBuffers.erase(client_fd);
}

void WinServer::pollLoop() {
    while (true) {
        int ret = WSAPoll(pollFds.data(), (ULONG)pollFds.size(), -1);
        if (ret == SOCKET_ERROR) {
            cerr << "WSAPoll failed: " << WSAGetLastError() << endl;
            continue;
        }

        for (size_t i = 0; i < pollFds.size(); i++) {
            auto &pfd = pollFds[i];

            if (pfd.revents == 0) continue;

            if (pfd.fd == server_fd && (pfd.revents & POLLRDNORM)) {
                sockaddr_in client_address;
                int addrlen = sizeof(client_address);
                SOCKET client_socket = accept(server_fd, (sockaddr *)&client_address, &addrlen);

                if (client_socket == INVALID_SOCKET) {
                    cerr << "Accept failed: " << WSAGetLastError() << endl;
                    continue;
                }

                WSAPOLLFD clientPollFd;
                clientPollFd.fd = client_socket;
                clientPollFd.events = POLLRDNORM | POLLWRNORM;
                clientPollFd.revents = 0;
                pollFds.push_back(clientPollFd);

                cout << "New client connected, fd = " << client_socket << endl;
            }
            else {
                if (pfd.revents & POLLRDNORM) {
                    handleRecv(pfd.fd);
                }
                if (pfd.revents & POLLWRNORM) {
                    handleSend(pfd.fd);
                }
                if (pfd.revents & (POLLERR | POLLHUP)) {
                    handleError(pfd.fd);
                }
            }
        }
    }
}

void WinServer::start() {
    pollThread = thread(&WinServer::pollLoop, this);
    pollThread.join(); // run on main thread
}

WinServer::~WinServer() {
    if (server_fd != INVALID_SOCKET) closesocket(server_fd);
    WSACleanup();
    if (pollThread.joinable()) pollThread.join();
}
