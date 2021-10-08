#include "event.h"
#include "eventsloop.h"

namespace Event
{
    const int Event::kNoneEvent = 0;
    const int Event::kReadEvent = POLLIN | POLLPRI;
    const int Event::kWriteEvent = POLLOUT;
    Event::Event(EventsLoop& loop, int fd)
    :loop_(loop), fd_(fd), events_(0), readCallback_(nullptr), writeCallback_(nullptr)
    {
        
    }
    
    void Event::update() 
    {
        loop.updateEvent(std::shared_from_this());
    }
}