#include "chan.h"

namespace Event
{
    Chan::chan(EventsLoop* loop, uint16_t port, Socket::SockType type, bool reuseport) 
    :loop_(loop),
    sock_(type),
    ev_(sock_.fd())
    {
        sock_.setReuseAddr(true);
        sock_.setReusePort(reuseport);
        sock_.bindAddress(Net::InetAddress(port));

        if(Socket::SockType::tcplink == type)
        {
            sock_.listen();
            listening_ = true;
        }      
    }

    Chan::~chan() 
    {
        ev_.disableAll();
        ev_.remove();
    }
}