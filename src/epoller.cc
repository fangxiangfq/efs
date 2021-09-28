#include "epoller.h"

namespace Event
{
    Epoller::Epoller() 
    {
        struct rlimit rl;
        if(getrlimit(RLIMIT_NOFILE, &rl) == 0 && rl.rlim_cur != RLIM_INFINITY)
            nfds_ = rl.rlim_cur;
        if(0 == nfds_ )
            nfds_ = 1024;
        if(nfds_ > 65535)
            nfds_ = 65535;
        if(0 > (epfd_ = epoll_create(1)))
        {
            //todo log
        }

        evsptr_ = std::make_unique<struct epoll_event[]>(nfds_);
    }
    
    bool Epoller::dispatch(int timeout) 
    {
        int num = epoll_wait(epfd_, evsprt_.get(), nfds_, timeout);
        if(0 > num)
        {
            if(errno != EINTR)
            {
                //todo log
                return false;
            }
            //todo signal
        }

        int event_ = 0;
        for(int i = 0; i < num; ++i)
	    {
            event_ = epevents[i].events;

        }

        return true;
    }
    
    bool epoll_base::add(std::shared_ptr<Event> ev)
    {
        struct epoll_event epev = {0};
        epev.data.fd = ev->fd;
        int op = EPOLL_CTL_ADD;

        if(ev->is_readable())
            epev.events |= (EPOLLIN | POLLPRI);
        if(ev->is_writeable())
            epev.events |= EPOLLOUT;

        if(epoll_ctl(epfd_, op, ev->fd, &epev) == -1)
        {
            return false;
        }

        return true;
    }

    bool Epoller::del(std::shared_ptr<Event> ev)
    {
        struct epoll_event epev = {0};
        epev.data.fd = ev->fd;

        int op = EPOLL_CTL_DEL;

        int events_ = 0;

        if(!ev->is_readable())
            events |= (EPOLLIN | POLLPRI);
        if(!ev->is_writeable())
            events |= EPOLLOUT;
        if(0 != events_)
        epev.events = events_;

        if(0 > epoll_ctl(epfd_, op, ev->fd, &epev))
        {
            //todo log
            return false;
        }

        return true;
    }
}