#include "eventsloop.h"
#include <unistd.h>
#include <sys/eventfd.h>
#include "logger.h"

namespace Event
{
    EventsLoop::EventsLoop()
    :looping_(false),
    quit_(false),
    tid_(std::this_thread::get_id()), 
    poller_(new Epoller()),
    wakeupEv_(new TaskEvent(this))
    {
        wakeupEv_->enableRead();
    }
    
    EventsLoop::~EventsLoop()
    {

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
            handlePostCbs();
        }
        //other thing
    }

    void EventsLoop::updateEvent(Event& event) 
    {
        assert(event.ownerLoop() == this);
        assertSelfThread();
        poller_->updateEvent(event);
    }
    
    void EventsLoop::removeEvent(Event& event) 
    {
        assert(event.ownerLoop() == this);
        assertSelfThread();
        poller_->removeEvent(event);
    }

    void EventsLoop::disableEvent(Event& event) 
    {
        poller_->disableEvent(event);
        STD_DEBUG("diable ev fd {}", event.fd());
    }

    void EventsLoop::enableEvent(Event& event) 
    {
        poller_->enableEvent(event);
        STD_DEBUG("enable ev fd {}", event.fd());
    }

    void EventsLoop::post(const PostCb& cb)
    {
        if (inLoopThread())
        {
            cb();
        }
        else
        {
            std::lock_guard<std::mutex> lock(mutex_);
            postCbs_.push_back(cb);
            wakeup();
        }
    }

    void EventsLoop::handlePostCbs()
    {
        std::vector<PostCb> postCbs;
        doingPostCb_ = true;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            postCbs.swap(postCbs_);
        }

        for (const PostCb& cb : postCbs)
        {
            cb();
        }

        doingPostCb_ = false;
    }
}