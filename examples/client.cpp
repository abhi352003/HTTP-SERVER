#include "tcpclient.h"
#include <iostream>
#include <string>

int main() {

    TCPClient * client =createClient();

    if (!client->connectToServer("127.0.0.1", 8080)) {
        std::cerr << "Client connection failed!" << std::endl;
        return -1;
    }

    std::cout << "Connected to server at 127.0.0.1:8080" << std::endl;

    // Send message
<<<<<<< HEAD
   
        std::string message = "Hello Server!";
        client->sendData(message);

    // Receive echo
        std::string response = client->receiveData();
        std::cout << "Server replied: " << response << std::endl;

    
    

=======
    int  x=10;
    while(x--)
        {
        std::string message = "Hello Server!";
        client->sendData(message);

    // Receive echo
        std::string response = client->receiveData();
        std::cout << "Server replied: " << response << std::endl;

    
    }

>>>>>>> 6b9f5c30c30e3216c705669f58b91ab5b49477d5
    client->disconnect();
    return 0;
}