#include "epoller.h"
#include <sys/resource.h>

namespace Event
{
    Epoller::Epoller() 
    {
        struct rlimit rl;
        if(getrlimit(RLIMIT_NOFILE, &rl) == 0 && rl.rlim_cur != RLIM_INFINITY)
            nfds_ = static_cast<int>(rl.rlim_cur);
        if(0 == nfds_ )
            nfds_ = 1024;
        if(nfds_ > 65535)
            nfds_ = 65535;
        if(0 > (epfd_ = epoll_create(EPOLL_CLOEXEC)))
        {
            //todo log
        }

        evsptr_ = std::make_unique<struct epoll_event[]>(nfds_);
    }
    
    void Epoller::updateEvent(const Event& event) 
    {
        struct epoll_event epev;
        epev.data.fd = event.fd();
        epev.events = event.events();
        int op = EPOLL_CTL_MOD;
        if(!hasEvent(event))
        {
            op = EPOLL_CTL_ADD;
            events_.emplace(event.fd(), event);
        }
            
        if (::epoll_ctl(epfd_, op, event.fd(), &epev) < 0)
        {
            //todo log
        }
    }
    
    void Epoller::removeEvent(const Event& event) 
    {
        struct epoll_event epev;
        epev.data.fd = event.fd();
        epev.events = event.events();
        int op = EPOLL_CTL_DEL;
        if(!hasEvent(event))
        {
           return;
        }

        events_.erase(event.fd());    
        if (::epoll_ctl(epfd_, op, event.fd(), &epev) < 0)
        {
            //todo log
        }
    }
    
    bool Epoller::dispatch(int timeout) 
    {
        int num = epoll_wait(epfd_, evsptr_.get(), nfds_, timeout);
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
            event_ = evsptr_[i].events;
            int fd = evsptr_[i].data.fd;
            if(events_.count(fd) <= 0)
                continue;
            const Event& ev = events_.at(fd);
            if(event_ & EPOLLIN)
            {
                ev.read();
            }
            //do something
        }

        return true;
    }
}