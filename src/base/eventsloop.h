#pragma once
#include <functional>
#include <pthread.h>
#include <thread>
#include <vector>
#include <map>
#include "epoller.h"
#include "event.h"
#include <atomic>
#include <assert.h>

namespace Event
{
    using TaskCb = std::function<void(int fd, size_t len)>;
    using TaskMap = std::map<short, TaskCb>;
    class EventsLoop
    {
    public:    
        EventsLoop();
        ~EventsLoop();  
        void loop();

        void updateEvent(const Event& event);
        void removeEvent(const Event& event);

        bool hasEvent(const Event& event)
        {
            if(poller_)
                return poller_->hasEvent(event); 
            return false;
        }

        void quit() { quit_ = true; }

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

        std::unique_ptr<Epoller> poller_;
    };
} // namespace Event

