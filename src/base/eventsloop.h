#pragma once
#include <functional>
#include <pthread.h>
#include <thread>
#include <vector>
#include <mutex>
#include <map>
#include "epoller.h"
#include "event.h"
#include <atomic>
#include <assert.h>

namespace Event
{
    using TaskCb = std::function<void(int fd, size_t len)>;
    using TaskMap = std::map<short, TaskCb>;
    
    using PostCb = std::function<void()>;
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

        bool inLoopThread() {return tid_ == std::this_thread::get_id();}

        void assertSelfThread()
        {
            if(tid_ != std::this_thread::get_id())
            {
                //todo log
                abort();
            }
        }
        
        void wakeup();
        void post(const PostCb& cb);
    private:
        void handlePostCbs();
        void handleWakeUp();
        
        bool looping_;
        std::atomic<bool> quit_;
        const std::thread::id tid_;
        std::unique_ptr<Epoller> poller_;

        int wakeupFd_;
        std::unique_ptr<Event> wakeupEv_;

        bool doingPostCb_; 
        std::mutex mutex_;
        std::vector<PostCb> postCbs_;
    };
} // namespace Event

