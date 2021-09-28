#pragma once
#include <functional>
#include <pthread.h>

namespace Event
{
    class Event;
    class Epoller;
    class EventsLoop
    {
    public:
        using functask =  std::function<void()>;
        EventsLoop();
        ~EventsLoop();  
        void loop();

        void updateEvent(Event* event);
        void removeEvent(Event* event);
        bool emptyEvent();
        void quit();
        
    private:
        void handletask();
        bool looping_;
        bool taskfd_;
        //tid
        std::unique_ptr<Epoller> poller_;
        std::unique_ptr<Event>   selfev_;
    };
} // namespace Event

