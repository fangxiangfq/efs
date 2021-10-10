#include "event.h"
#include "eventsloop.h"

namespace Event
{
    const int Event::kNoneEvent = 0;
    const int Event::kReadEvent = EPOLLIN | EPOLLPRI;
    const int Event::kWriteEvent = EPOLLOUT;
    Event::Event(EventsLoop* loop, int fd)
    :loop_(loop), fd_(fd), events_(0), readCallback_(nullptr), writeCallback_(nullptr)
    {
        
    }
    
    void Event::update() 
    {
        loop_->updateEvent(shared_from_this());
    }
}