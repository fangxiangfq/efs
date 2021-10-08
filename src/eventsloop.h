#pragma once
#include <functional>
#include <pthread.h>
#include "event.h"

namespace Event
{
    class Event;
    class Epoller;
    using functask = std::function<void()>;
    class EventsLoop
    {
    public:    
        EventsLoop();
        ~EventsLoop();  
        void loop();

        void updateEvent(EventPtr event);
        void removeEvent(EventPtr event);
        bool empty();
        void quit();
        
    private:
        void handleTask();
        bool looping_;
        bool taskfd_;
        //tid
        std::unique_ptr<Epoller> poller_;
        std::unique_ptr<Event>   selfev_;
    };
} // namespace Event

