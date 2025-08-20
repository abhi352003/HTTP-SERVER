#ifndef TCPSERVER_H
#define TCPSERVER_H

#include<string>

class TCPServer{
    public:
    virtual bool initialize(int port,const std::string &ipAddress = "127.0.0.1") = 0;
    virtual void start() = 0;
    virtual ~TCPServer() = default;

};

TCPServer *createserver();



#endif