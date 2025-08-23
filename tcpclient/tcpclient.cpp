#include "tcpclient.h"

#ifdef PLATFORM_WINDOWS
#include "win/win.h"
#elif defined(PLATFORM_LINUX)
#include "lin/lin.h"
#elif defined(PLATFORM_MAC)
#include "mac/mac.h"
#endif

TCPClient *createClient()
{
#ifdef PLATFORM_WINDOWS
    return new WinClient();
#elif defined(PLATFORM_LINUX)
    return new LinClient();
#elif defined(PLATFORM_MAC)
    return new MacClient();
#else
    return nullptr; // In case no platform is defined
#endif
<<<<<<< HEAD
}
=======
}
>>>>>>> 6b9f5c30c30e3216c705669f58b91ab5b49477d5
