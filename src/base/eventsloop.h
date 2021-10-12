#pragma once
#include <functional>
#include <pthread.h>
#include <thread>
#include <vector>
#include <map>
#include "epoller.h"
#include "event.h"
#include <atomic>

namespace Event
{
    using taskcb = std::function<void(int fd, size_t len)>;
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
        void quit(){quit_ = true;};
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
        std::atomic<bool> quit_;
        const std::thread::id tid_;

        const int taskfd_;
        std::unique_ptr<Event> taskev_;

        std::map<short, taskcb> taskmap_;
        std::unique_ptr<Epoller> poller_;
    };
} // namespace Event

