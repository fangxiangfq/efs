#pragma once 
#include "event.h"
#include "sockets.h"
#include <map>

namespace Event
{
    class Chan
    {
    public:
        Chan(EventsLoop* loop, uint16_t port, Socket::SockType type = Socket::SockType::udp);
        ~Chan();
    private:
        Socket::Socket sock_;
        Event   ev_;
        bool    waiting_;
        bool    listening_{false};
    };

    using ChansMap = std::map<int, std::unique_ptr<Chan>>;
}
        
        
