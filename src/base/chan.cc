#include "chan.h"
#include "inetaddress.h"

namespace Event
{
    Chan::Chan(uint16_t port, EventsLoop* loop, Socket::SockType type) 
    :sock_(type)
    // ev_(loop, sock_.fd())
    {
        sock_.setReuseAddr(true);
        sock_.bindAddress(Net::InetAddress(port));

        if(Socket::SockType::tcplisten == type)
        {
            sock_.listen();
            listening_ = true;
        } 

        // ev_.enableRead();    
    }

    // Chan::Chan(int connfd, EventsLoop* loop, Socket::SockType type) 
    // :sock_(connfd, type)
    // // ev_(loop, sock_.fd())
    // {
    //     sock_.setReuseAddr(true);  
    //     // ev_.enableRead(); 
    // }

    Chan::~Chan() 
    {
        // ev_.disableAll();
        // ev_.remove();
    }
}