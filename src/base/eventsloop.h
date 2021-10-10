#pragma once
#include <functional>
#include <pthread.h>
#include <thread>
#include <vector>
#include "epoller.h"
#include "event.h"

namespace Event
{
    using functask = std::function<void()>;
    class EventsLoop
    {
    public:    
        EventsLoop();
        EventsLoop(int taskfd);
        ~EventsLoop();  
        void loop();

        void updateEvent(EventPtr event);
        void removeEvent(EventPtr event);
        bool hasEvent(EventPtr event)
        {
            if(poller_)
                return poller_->hasEvent(event); 
            return false;
        }
        bool empty();
        void quit();
        void assertSelfThread()
        {
            if(tid_ != std::this_thread::get_id())
            {
                //todo log
                abort();
            }
        }
    private:
        void handleTask();
        bool looping_;
        const std::thread::id tid_;
        bool main_loop_;
        const int taskfd_;
        std::unique_ptr<Event> taskev_;

        std::unique_ptr<Epoller> poller_;
        std::vector<int> worktaskfds_;
    };
} // namespace Event

