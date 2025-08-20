#include "lin.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>   
#include <cstring>    
#include <cerrno>     
using namespace std;

LinServer::LinServer() : server_fd(-1){}


bool LinServer::initialize(int port , const std::string &ip_address){
    cout<<"initializing the server ..." << endl;
    struct sockaddr_in address;
    int opt = 1;

    // Create socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cerr << "Socket creation failed: " << strerror(errno) << endl;
        return false;
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)) < 0) {
        cerr << "setsockopt failed: " << strerror(errno) << endl;
        close(server_fd);
        return false;
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (ip_address == "0.0.0.0") {
        // Accept connections from any interface
        address.sin_addr.s_addr = INADDR_ANY;   
    } else {
        if (inet_pton(AF_INET, ip_address.c_str(), &address.sin_addr) <= 0) {
            cerr << "Invalid IP address: " << ip_address << endl;
            close(server_fd);
            return false;
        }
    }


    
    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed: " << strerror(errno) << endl;
        close(server_fd);
        return false;
    }

    // Start listening
    if (listen(server_fd, SOMAXCONN) < 0) {
        cerr << "Listen failed: " << strerror(errno) << endl;
        close(server_fd);
        return false;
    }


     cout << "Server initialized successfully on " << ip_address
         << ":" << port << endl;

    return true;

}


void LinServer::start(){

    struct sockaddr_in client_address;
    socklen_t addrlen = sizeof(client_address);
    while(true)
    {
        if((client_socket = accept(server_fd,(struct sockaddr *)&client_address, &addrlen))<0)
        {
            std::cerr <<"Accept failed ! Error: " <<strerror(errno) << endl;
            continue;
        }
    }
}

