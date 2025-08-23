#ifndef WIN_CLIENT_H
#define WIN_CLIENT_H

#include "../tcpclient.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

class WinClient : public TCPClient
{
public:
    WinClient();
    ~WinClient();

    bool connectToServer(const std::string &ip, int port) override;
    bool sendData(const std::string &data) override;
    std::string receiveData() override;
    void disconnect() override;

private:
    SOCKET sockfd;
    WSADATA wsaData;
};

<<<<<<< HEAD
#endif 
=======
#endif
>>>>>>> 6b9f5c30c30e3216c705669f58b91ab5b49477d5
