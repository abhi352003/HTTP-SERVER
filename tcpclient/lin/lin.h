#ifndef LIN_CLIENT_H
#define LIN_CLIENT_H

#include "../tcpclient.h"
#include <string>

class LinClient : public TCPClient
{
public:
    LinClient();
    ~LinClient();

    bool connectToServer(const std::string &ip, int port) override;
    bool sendData(const std::string &data) override;
    std::string receiveData() override;
    void disconnect() override;

private:
    int sockfd;
};

<<<<<<< HEAD
#endif 
=======
#endif
>>>>>>> 6b9f5c30c30e3216c705669f58b91ab5b49477d5
