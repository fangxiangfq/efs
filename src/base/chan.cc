#include "chan.h"
#include "inetaddress.h"

namespace Event
{
    Chan::Chan(EventsLoop* loop, uint16_t port, Socket::SockType type) 
    :sock_(type),
    ev_(loop, sock_.fd())
    {
        sock_.setReuseAddr(true);
        sock_.bindAddress(Net::InetAddress(port));

        if(Socket::SockType::tcplink == type)
        {
            sock_.listen();
            listening_ = true;
        }      
    }

    Chan::Chan(int connfd, EventsLoop* loop, Socket::SockType type) 
    :sock_(connfd, type),
    ev_(loop, sock_.fd())
    {
        sock_.setReuseAddr(true);  
    }

    Chan::~Chan() 
    {
        ev_.disableAll();
        ev_.remove();
    }
}