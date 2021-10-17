#include "event.h"
#include "eventsloop.h"
#include <assert.h>
namespace Event
{
    const int Event::kNoneEvent = 0;
    const int Event::kReadEvent = EPOLLIN | EPOLLPRI;
    const int Event::kWriteEvent = EPOLLOUT;
    Event::Event(EventsLoop* loop, int fd, EvType type, uint16_t port)
    :loop_(loop), fd_(fd), events_(0), readCallback_(nullptr), writeCallback_(nullptr), type_(type), port_(port)
    {
        
    }
    
    void Event::remove()
    {
        assert(isNoneEvent());
        loop_->removeEvent(*this);
    }

    void Event::update() 
    {
        //loop_->updateEvent(shared_from_this());
        loop_->updateEvent(*this);
    }

}