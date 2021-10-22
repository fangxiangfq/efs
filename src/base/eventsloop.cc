#include "eventsloop.h"
#include <unistd.h>
#include <sys/eventfd.h>

namespace Event
{
    EventsLoop::EventsLoop()
    :looping_(false),
    quit_(false),
    tid_(std::this_thread::get_id()), 
    poller_(new Epoller()),
    wakeupFd_(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)),
    wakeupEv_(new Event(this, wakeupFd_))
    {
        if(wakeupFd_ < 0)
        {
            //todo log
            abort();
        }
        wakeupEv_->setRead(std::bind(&EventsLoop::handleWakeUp, this));
        wakeupEv_->enableRead();
    }
    
    EventsLoop::~EventsLoop()
    {
        ::close(wakeupFd_);
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

    void EventsLoop::updateEvent(const Event& event) 
    {
        assert(event->loop() == this);
        assertSelfThread();
        poller_->updateEvent(event);
    }
    
    void EventsLoop::removeEvent(const Event& event) 
    {
        assert(event->loop() == this);
        assertSelfThread();
        poller_->removeEvent(event);
    }

    void EventsLoop::wakeup()
    {
        uint64_t one = 1;
        ssize_t n = ::write(wakeupFd_, &one, sizeof one);
        if (n != sizeof one)
        {
            
        }
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

    void EventsLoop::handleWakeUp()
    {
        uint64_t one = 1;
        ssize_t n = ::read(wakeupFd_, &one, sizeof one);
        if (n != sizeof one)
        {
            
        }
    }
}