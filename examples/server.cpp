#include "../tcpserver/win/win.h"   // adjust depending on OS

int main() {
    WinServer server;  // Or LinServer / MacServer
    if (!server.initialize(8080, "127.0.0.1")) {
        return -1;
    }
    server.start();
    return 0;
}
