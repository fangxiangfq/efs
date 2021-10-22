#include "threadlocal.h"

namespace ThreadLocal
{
    std::atomic<uint32_t> ThreadLocal::nextSlotId_;
    thread_local ThreadLocal::ThreadLocalData ThreadLocal::threadLocalData_;
    std::set<Event::EventsLoop*> ThreadLocal::registeredThreads_;

    ThreadLocalObjectSharedPtr ThreadLocal::get(uint32_t index) 
    {
        assert(threadLocalData_.find(index) != threadLocalData_.end());
        return threadLocalData_.data_[index];
    }
    
    void ThreadLocal::registerThread(Event::EventsLoop* loop, bool mainThread) 
    {
        if (mainThread) 
        {
            mainLoop_ = loop;
        } 
        else 
        {
            assert(!registeredThreads_.count(loop));
            registeredThreads_.insert(loop);
        }
    }
    
    void ThreadLocal::runOnAllThreads(PostCb cb) 
    {
        assert(std::this_thread::get_id() == mainThreadId_);
        // for (Event::EventsLoop* loop : registeredThreads_) 
        // {
        //     //loop->post(cb);
        // }

        cb();
    }
    
    void ThreadLocal::set(uint32_t index, InitializeCb cb) 
    {
        assert(std::this_thread::get_id() == mainThreadId_);
        // for (Event::EventsLoop* loop : registeredThreads_)
        // {
            
        // }

        threadLocalData_.data_[index] = cb(mainLoop_);
    }
    
    void ThreadLocal::shutdownThread() 
    {
        for (auto& entry : threadLocalData_.data_) 
        {
            entry.second->shutdown();
        }
    }
    
    void ThreadLocal::reset() 
    {
        assert(std::this_thread::get_id() == mainThreadId_);
        nextSlotId_ = 0;
        threadLocalData_.data_.clear();
        registeredThreads_.clear();
    }
}