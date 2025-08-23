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

<<<<<<< HEAD
#endif // MAC_CLIENT_H
=======
#endif // MAC_CLIENT_H
>>>>>>> 6b9f5c30c30e3216c705669f58b91ab5b49477d5
