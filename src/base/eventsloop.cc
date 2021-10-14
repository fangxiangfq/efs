#include "eventsloop.h"

namespace Event
{
    EventsLoop::EventsLoop(int taskfd, const TaskMap& taskmap)
    :looping_(false),
    quit_(false),
    tid_(std::this_thread::get_id()), 
    taskfd_(taskfd),
    taskmap_(taskmap),
    taskev_(new Event(this, taskfd_)),
    poller_(new Epoller())
    {
        
    }
    
    EventsLoop::~EventsLoop()
    {
        //todo
    }

    void EventsLoop::loop()
    {
        assert(!looping_);
        assertSelfThread();
        looping_ = true;
        while(!quit_)
        {
            if(false == poller_->dispatch(-1))
            {
                //todo log
                abort();
            }
        }
        //other thing
    }

    void EventsLoop::updateEvent(EventPtr event) 
    {
        assert(event->loop() == this);
        assertSelfThread();
        poller_->updateEvent(event);
    }
    
    void EventsLoop::removeEvent(EventPtr event) 
    {
        assert(event->loop() == this);
        assertSelfThread();
        poller_->removeEvent(event);
    }
}