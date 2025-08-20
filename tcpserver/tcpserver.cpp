#include "tcpserver.h"

#ifdef PLATFORM_WINDOWS
#include "win/win.h"
#elif defined(PLATFORM_LINUX) 
#include "lin/lin.h"
#elif defined(PLATFORM_MAC)
#include "mac/mac.h"
#endif

TCPServer *createserver()
{

#ifdef PLATFORM_WINDOWS
    return new WinServer();
#elif defined(PLATFORM_LINUX) 
    return new LinServer();
#elif defined(PLATFORM_MAC)
    return new MacServer();
#endif

}

