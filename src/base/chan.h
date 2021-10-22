#pragma once 
#include "event.h"
#include "sockets.h"
#include <map>
#include <vector>

namespace Event
{
    class Chan
    {
    public:
        Chan(EventsLoop* loop, uint16_t port, Socket::SockType type = Socket::SockType::udp);
        Chan(int fd, EventsLoop* loop, Socket::SockType type = Socket::SockType::tcp);
        ~Chan();
    private:
        Socket::Socket sock_;
        Event   ev_;
        bool    listening_{false};
    };

    using ChansMap = std::map<int, std::unique_ptr<Chan>>;

    class Local
    {
    public:
        Local()
        :sockp_(), masterEv_(sockp_.first()), wokerEv_(sockp_.second()){}
        ~Local()=default;
        Socket::SocketPair& getSockp() {return sockp_;}
        Event& getMasterEv() {return masterEv_;}
        Event& getWorkerEv() {return wokerEv_;}
        bool working() const { return working_;}
        void setWorking(bool on)
        {
            working_ = on;
        }

    private:
        Socket::SocketPair sockp_;
        Event   masterEv_;
        Event   wokerEv_;
        bool    working_{false};
    };

    using LocalPtr = std::unique_ptr<Local>;
    using LocalsArr = std::vector<LocalPtr>;
}
        
        
