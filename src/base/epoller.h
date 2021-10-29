#pragma once
#include <sys/epoll.h>
#include <memory>
#include <map>
#include "event.h"

namespace Event
{
    using Evsptr = std::unique_ptr<struct epoll_event[]>;
    class Epoller
    {
        public:
        Epoller();
        ~Epoller(){};
        //no need to use shared pointer
        void updateEvent(Event& event);
        void removeEvent(Event& event);
        bool hasEvent(Event& event){return events_.count(event.fd()) > 0;}
        bool dispatch(int timeout);

    private:
        int epfd_;
        int nfds_;
        Evsptr evsptr_;
        std::map<int, Event&> events_;
    };
}

