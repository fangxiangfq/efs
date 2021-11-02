#include "workerfactory.h"
#include <assert.h>
namespace Thread
{
    WorkerFactory::WorkerFactory(Event::EventsLoop* baseLoop, const std::string& nameArg, uint16_t threadNum)
    :baseLoop_(baseLoop),
    name_(nameArg),
    started_(false),
    threadNum_(threadNum),
    workerIdx(0),
    threads_(threadNum_)
    {
        init();
    }
    
    WorkerFactory::~WorkerFactory() 
    {
        // Don't delete loop, it's stack variable
    }
    
    void WorkerFactory::init() 
    {
        assert(!started_);
        for (int i = 0; i < threadNum_; ++i) 
        { 
            workertaskEv_.emplace_back(EvManager::createTaskEvPtr());
            mastertaskEv_.emplace_back(EvManager::createTaskEvPtr());
        }
    }
    
    void WorkerFactory::start(const WorkInitCb& cb) 
    {
        assert(!started_);
        baseLoop_->assertSelfThread();

        started_ = true;

        for (int i = 0; i < threadNum_; ++i)
        {
            char buf[name_.size() + 32];
            snprintf(buf, sizeof buf, "%s%d", name_.c_str(), i);
            std::unique_ptr<Thread> worker(new Thread(workertaskEv_[i], cb, std::string(buf)));
            loops_.push_back(worker->startLoop());
            threads_.push_back(std::move(worker));
        }

        if (0 == threadNum_ && cb)
        {
            cb(baseLoop_);
        }
    }
    
    Event::EventsLoop* WorkerFactory::getNextLoop() 
    {
        baseLoop_->assertSelfThread();
        assert(started_);
        Event::EventsLoop* loop = baseLoop_;

        if(!loops_.empty())
        {
            loop = loops_[workerIdx];
            ++workerIdx;
            if (workerIdx >= loops_.size())
            {
                workerIdx = 0;
            }
        }

        return loop;
    }
    
    Event::EventsLoop* WorkerFactory::getLoopForHash(size_t hashCode) 
    {
        baseLoop_->assertSelfThread();
        Event::EventsLoop* loop = baseLoop_;

        if (!loops_.empty())
        {
            loop = loops_[hashCode % loops_.size()];
        }
        return loop;
    }
}