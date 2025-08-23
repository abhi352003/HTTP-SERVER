#ifndef MAC_CLIENT_H
#define MAC_CLIENT_H

#include "../tcpclient.h"
#include <string>

class MacClient : public TCPClient
{
public:
    MacClient();
    ~MacClient();

    bool connectToServer(const std::string &ip, int port) override;
    bool sendData(const std::string &data) override;
    std::string receiveData() override;
    void disconnect() override;

private:
    int sockfd;
};

#endif // MAC_CLIENT_H