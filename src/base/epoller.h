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
        void updateEvent(EventPtr event);
        void removeEvent(EventPtr event);
        bool hasEvent(EventPtr event){return events_.count(event->fd()) > 0;}
        bool dispatch(int timeout);

    private:
        int epfd_;
        int nfds_;
        Evsptr evsptr_;
        std::map<int, EventPtr> events_;
    };
}

