#pragma once
#include <functional>
#include <pthread.h>
#include <thread>
#include <vector>
#include <mutex>
#include <map>
#include <atomic>
#include <assert.h>
#include "epoller.h"
#include "event.h"
#include "evmanager.h"

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

        void updateEvent(Event& event);
        void removeEvent(Event& event);

        void disableEvent(Event& event);//must thread safe
        void enableEvent(Event& event);//must thread safe

        bool hasEvent(Event& event)
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

        void wakeup() { uint64_t one = 1; wakeupEv_->write(one); };
        void post(const PostCb& cb);
    private:
        void handlePostCbs();
   
        bool looping_;
        std::atomic<bool> quit_;
        const std::thread::id tid_;
        std::unique_ptr<Epoller> poller_;

        TaskEvPtr wakeupEv_;

        bool doingPostCb_; 
        std::mutex mutex_;
        std::vector<PostCb> postCbs_;
    };
} // namespace Event

