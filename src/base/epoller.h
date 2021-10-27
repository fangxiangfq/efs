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
        void updateEvent(const Event& event);
        void removeEvent(const Event& event);
        bool hasEvent(const Event& event){return events_.count(event.fd()) > 0;}
        bool dispatch(int timeout);

    private:
        int epfd_;
        int nfds_;
        Evsptr evsptr_;
        std::map<int, const Event&> events_;
    };
}

