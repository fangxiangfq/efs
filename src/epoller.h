#pragma once
#include <sys/epoll.h>
#include <memory>

namespace Event
{
    using evsptr = std::unique_ptr<struct epoll_event[]>;
    class Epoller
    {
        public:
        Epoller();
        ~Epoller(){};

        bool dispatch(int timeout);

    private:
        int epfd_;
        int nfds_;
        evsptr evsptr_;
    };
}

