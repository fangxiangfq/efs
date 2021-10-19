#include "threadpool.h"
#include <assert.h>

namespace Thread
{
    ThreadPool::ThreadPool(Event::EventsLoop* baseLoop, const std::string& nameArg)
    :baseLoop_(baseLoop),
    name_(nameArg),
    started_(false),
    numThreads_(1),
    next_(0)
    {
        
    }
    
    ThreadPool::~ThreadPool() 
    {
        // Don't delete loop, it's stack variable
    }
    
    void ThreadPool::start(const Event::LocalsArr& localsArr, const ThreadInitCallback& cb) 
    {
        assert(!started_);
        assert(threadFds.size() >= static_cast<size_t>(numThreads_));
        baseLoop_->assertSelfThread();

        started_ = true;

        for (int i = 0; i < numThreads_; ++i)
        {
            char buf[name_.size() + 32];
            snprintf(buf, sizeof buf, "%s%d", name_.c_str(), i);
            std::unique_ptr<Thread> worker = std::make_unique<Thread>(localsArr[i]->getMasterEv(), cb, std::string(buf));
            loops_.push_back(worker->startLoop());
            threads_.push_back(std::move(worker));
        }

        if (0 == numThreads_ && cb)
        {
            cb(baseLoop_);
        }
    }
    
    Event::EventsLoop* ThreadPool::getNextLoop() 
    {
        baseLoop_->assertSelfThread();
        assert(started_);
        Event::EventsLoop* loop = baseLoop_;

        if(!loops_.empty())
        {
            loop = loops_[next_];
            ++next_;
            if (next_ >= loops_.size())
            {
                next_ = 0;
            }
        }

        return loop;
    }
    
    Event::EventsLoop* ThreadPool::getLoopForHash(size_t hashCode) 
    {
        baseLoop_->assertSelfThread();
        Event::EventsLoop* loop = baseLoop_;

        if (!loops_.empty())
        {
            loop = loops_[hashCode % loops_.size()];
        }
        return loop;
    }
    
    int ThreadPool::getNextLoopIndex() 
    {
        int idx = -1;
        if(!loops_.empty())
        {
            idx = static_cast<int>(next_);
            ++next_;
            if (next_ >= loops_.size())
            {
                next_ = 0;
            }
        }
        return idx;
    }

    std::vector<Event::EventsLoop*> ThreadPool::getAllLoops()
    {
        baseLoop_->assertSelfThread();
        assert(started_);
        if (loops_.empty())
        {
            return std::vector<Event::EventsLoop*>(1, baseLoop_);
        }
        else
        {
            return loops_;
        }
    }
}