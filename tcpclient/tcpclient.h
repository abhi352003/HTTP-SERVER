#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <string>

class TCPClient
{
public:
    virtual bool connectToServer(const std::string &ip, int port) = 0;
    virtual bool sendData(const std::string &data) = 0;
    virtual std::string receiveData() = 0;
    virtual void disconnect() = 0;

    virtual ~TCPClient() = default;
};

TCPClient *createClient();

#endif 