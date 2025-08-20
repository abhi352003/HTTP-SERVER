#ifndef LINSERVER_H
#define LINSERVER_H

#include "./tcpserver.h"
#include<iostream>

class LinServer : public TCPServer {
    private:
    int server_fd;
    public:
     LinServer();
     bool initialize(int port,const std::string &ipAddress = "127.0.0.1") override;
     void start() override;
    virtual ~LinServer();

};



#endif