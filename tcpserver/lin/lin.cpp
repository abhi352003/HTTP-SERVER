#include "lin.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <unordered_map>
using namespace std;

LinServer::LinServer() : server_fd(-1), epoll_fd(-1) {}

bool LinServer::initialize(int port, const std::string &ip_address)
{
    cout << "initializing the server ..." << endl;
    struct sockaddr_in address;
    int opt = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        cerr << "Socket creation failed: " << strerror(errno) << endl;
        return false;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0 ||
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0)
    {
        cerr << "setsockopt failed: " << strerror(errno) << endl;
        close(server_fd);
        return false;
    }

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        cerr << "Epoll creation failed: " << strerror(errno) << endl;
        close(server_fd);
        return false;
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (ip_address == "0.0.0.0")
    {
        address.sin_addr.s_addr = INADDR_ANY;
    }
    else
    {
        if (inet_pton(AF_INET, ip_address.c_str(), &address.sin_addr) <= 0)
        {
            cerr << "Invalid IP address: " << ip_address << endl;
            close(server_fd);
            return false;
        }
    }

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        cerr << "Bind failed: " << strerror(errno) << endl;
        close(server_fd);
        return false;
    }

    if (listen(server_fd, SOMAXCONN) < 0)
    {
        cerr << "Listen failed: " << strerror(errno) << endl;
        close(server_fd);
        return false;
    }

    cout << "Server initialized successfully on " << ip_address << ":" << port << endl;
    return true;
}

bool LinServer::setSocketNonBlocking(int sockfd)
{
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
    {
        cerr << "fcntl get failed: " << strerror(errno) << endl;
        return false;
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        cerr << "fcntl set failed: " << strerror(errno) << endl;
        return false;
    }

    return true;
}


void LinServer::handleRecv(int client_fd)
{
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];

    while (true)
    {
        int byteRead = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (byteRead > 0)
        {
            buffer[byteRead] = '\0';
            cout << "Received (" << byteRead << " bytes) from fd " << client_fd << ": " << buffer << endl;

            // Echo back -> put into buffer
            sendBuffers[client_fd] += buffer;

            // Enable EPOLLOUT so handleSend() can flush it
            struct epoll_event ev;
            ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
            ev.data.fd = client_fd;
            epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &ev);
        }
        else if (byteRead == 0)
        {
            cout << "Client disconnected (fd " << client_fd << ")" << endl;
            close(client_fd);
            sendBuffers.erase(client_fd);
            break;
        }
        else
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break; // No more data
            }
            else
            {
                cerr << "Recv failed: " << strerror(errno) << endl;
                close(client_fd);
                sendBuffers.erase(client_fd);
                break;
            }
        }
    }
}

void LinServer::handleSend(int client_fd)
{
    auto it = sendBuffers.find(client_fd);
    if (it == sendBuffers.end() || it->second.empty())
        return;

    std::string &msg = it->second;
    
    ssize_t bytesSent = send(client_fd, msg.c_str(), msg.size(), 0);

    if (bytesSent > 0)
    {
        msg.erase(0, bytesSent);
        cout << "Sent " << bytesSent << " bytes to fd " << client_fd << endl;
    }
    else if (bytesSent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
    {
        return; // try again later
    }
    else
    {
        cerr << "Send failed: " << strerror(errno) << endl;
        close(client_fd);
        sendBuffers.erase(client_fd);
        return;
    }

    // If buffer empty, disable EPOLLOUT
    if (msg.empty())
    {
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = client_fd;
        epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &ev);
    }
}

void LinServer::handleError(int client_fd)
{
    int error = 0;
    socklen_t len = sizeof(error);
    if (getsockopt(client_fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
    {
        cerr << "getsockopt failed for fd " << client_fd << ": " << strerror(errno) << endl;
    }
    else
    {
        cerr << "Socket error on fd " << client_fd << ": " << strerror(error) << endl;
    }

    close(client_fd);
    sendBuffers.erase(client_fd);
}

void LinServer::epollLoop()
{
    const int MAX_EVENTS = 10;
    struct epoll_event events[MAX_EVENTS];

    while (true)
    {
        int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (event_count < 0)
        {
            cerr << "epoll_wait error: " << strerror(errno) << endl;
            continue;
        }

        for (int i = 0; i < event_count; i++)
        {
            int fd = events[i].data.fd;

            if (events[i].events & EPOLLIN)
            {
                handleRecv(fd);
            }
            if (events[i].events & EPOLLOUT)
            {
                handleSend(fd);
            }
            if (events[i].events & (EPOLLERR | EPOLLHUP))
            {
                handleError(fd);
            }
        }
    }
}

void LinServer::start()
{
    epollThread = std::thread(&LinServer::epollLoop, this);

    struct sockaddr_in client_address;
    socklen_t addrlen = sizeof(client_address);

    while (true)
    {
        int client_socket = accept(server_fd, (struct sockaddr *)&client_address, &addrlen);
        if (client_socket < 0)
        {
            cerr << "Accept failed: " << strerror(errno) << endl;
            continue;
        }

        setSocketNonBlocking(client_socket);

        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET; // only listen for read initially
        ev.data.fd = client_socket;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &ev) == -1)
        {
            cerr << "epoll_ctl add failed: " << strerror(errno) << endl;
            close(client_socket);
        }

        cout << "New client connected, fd = " << client_socket << endl;
    }
}

LinServer::~LinServer()
{
    if (server_fd != -1) close(server_fd);
    if (epoll_fd != -1) close(epoll_fd);
    if (epollThread.joinable()) epollThread.join();
}
