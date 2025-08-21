#include "mac.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/event.h>
#include <cstring>
#include <cerrno>

using namespace std;

MacServer::MacServer() : server_fd(-1), kqueue_fd(-1) {}

bool MacServer::initialize(int port, const std::string &ipAddress) {
    cout << "Initializing Mac server..." << endl;
    struct sockaddr_in address;
    int opt = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cerr << "Socket creation failed: " << strerror(errno) << endl;
        return false;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        cerr << "setsockopt failed: " << strerror(errno) << endl;
        close(server_fd);
        return false;
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (ipAddress == "0.0.0.0") {
        address.sin_addr.s_addr = INADDR_ANY;
    } else {
        if (inet_pton(AF_INET, ipAddress.c_str(), &address.sin_addr) <= 0) {
            cerr << "Invalid IP address: " << ipAddress << endl;
            close(server_fd);
            return false;
        }
    }

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        cerr << "Bind failed: " << strerror(errno) << endl;
        close(server_fd);
        return false;
    }

    if (listen(server_fd, SOMAXCONN) < 0) {
        cerr << "Listen failed: " << strerror(errno) << endl;
        close(server_fd);
        return false;
    }

    kqueue_fd = kqueue();
    if (kqueue_fd == -1) {
        cerr << "kqueue creation failed: " << strerror(errno) << endl;
        close(server_fd);
        return false;
    }

    cout << "Mac server initialized successfully on " << ipAddress << ":" << port << endl;
    return true;
}

bool MacServer::setSocketNonBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        cerr << "fcntl get failed: " << strerror(errno) << endl;
        return false;
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        cerr << "fcntl set failed: " << strerror(errno) << endl;
        return false;
    }

    return true;
}

void MacServer::handleRecv(int client_fd) {
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];

    while (true) {
        int byteRead = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (byteRead > 0) {
            buffer[byteRead] = '\0';
            cout << "Received (" << byteRead << " bytes) from fd " << client_fd << ": " << buffer << endl;

            sendBuffers[client_fd] += buffer;

            struct kevent ev;
            EV_SET(&ev, client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, nullptr);
            kevent(kqueue_fd, &ev, 1, nullptr, 0, nullptr);
        } else if (byteRead == 0) {
            cout << "Client disconnected (fd " << client_fd << ")" << endl;
            close(client_fd);
            sendBuffers.erase(client_fd);
            break;
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                cerr << "Recv failed: " << strerror(errno) << endl;
                close(client_fd);
                sendBuffers.erase(client_fd);
                break;
            }
        }
    }
}

void MacServer::handleSend(int client_fd) {
    auto it = sendBuffers.find(client_fd);
    if (it == sendBuffers.end() || it->second.empty())
        return;

    string &msg = it->second;

    while (!msg.empty()) {
        ssize_t bytesSent = send(client_fd, msg.c_str(), msg.size(), 0);
        if (bytesSent > 0) {
            msg.erase(0, bytesSent);
            cout << "Sent " << bytesSent << " bytes to fd " << client_fd << endl;
        } else if (bytesSent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            return;
        } else {
            cerr << "Send failed: " << strerror(errno) << endl;
            close(client_fd);
            sendBuffers.erase(client_fd);
            return;
        }
    }

    // disable write event if buffer empty
    if (msg.empty()) {
        struct kevent ev;
        EV_SET(&ev, client_fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
        kevent(kqueue_fd, &ev, 1, nullptr, 0, nullptr);
    }
}

void MacServer::handleError(int client_fd) {
    cerr << "Socket error on fd " << client_fd << endl;
    close(client_fd);
    sendBuffers.erase(client_fd);
}

void MacServer::kqueueLoop() {
    const int MAX_EVENTS = 10;
    struct kevent events[MAX_EVENTS];

    while (true) {
        int nev = kevent(kqueue_fd, nullptr, 0, events, MAX_EVENTS, nullptr);
        if (nev < 0) {
            cerr << "kevent error: " << strerror(errno) << endl;
            continue;
        }

        for (int i = 0; i < nev; i++) {
            int fd = events[i].ident;

            if (events[i].filter == EVFILT_READ) {
                handleRecv(fd);
            } else if (events[i].filter == EVFILT_WRITE) {
                handleSend(fd);
            }

            if (events[i].flags & EV_EOF) {
                handleError(fd);
            }
        }
    }
}

void MacServer::start() {
    kqueueThread = thread(&MacServer::kqueueLoop, this);

    struct sockaddr_in client_address;
    socklen_t addrlen = sizeof(client_address);

    while (true) {
        int client_socket = accept(server_fd, (struct sockaddr *)&client_address, &addrlen);
        if (client_socket < 0) {
            cerr << "Accept failed: " << strerror(errno) << endl;
            continue;
        }

        setSocketNonBlocking(client_socket);

        struct kevent ev;
        EV_SET(&ev, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
        if (kevent(kqueue_fd, &ev, 1, nullptr, 0, nullptr) == -1) {
            cerr << "kevent add failed: " << strerror(errno) << endl;
            close(client_socket);
        }

        cout << "New client connected, fd = " << client_socket << endl;
    }
}

MacServer::~MacServer() {
    if (server_fd != -1) close(server_fd);
    if (kqueue_fd != -1) close(kqueue_fd);
    if (kqueueThread.joinable()) kqueueThread.join();
}
