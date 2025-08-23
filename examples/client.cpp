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
   
        std::string message = "Hello Server!";
        client->sendData(message);

    // Receive echo
        std::string response = client->receiveData();
        std::cout << "Server replied: " << response << std::endl;

    
    

    client->disconnect();
    return 0;
}
