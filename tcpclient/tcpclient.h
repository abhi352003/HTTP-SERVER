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

<<<<<<< HEAD
#endif // TCPCLIENT_H
=======
#endif 
>>>>>>> 6b9f5c30c30e3216c705669f58b91ab5b49477d5
