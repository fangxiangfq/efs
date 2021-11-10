#include "threadlocal.h"

namespace ThreadLocal
{
    thread_local ThreadLocal::ThreadLocalData ThreadLocal::threadLocalData_;
    
    SlotPtr ThreadLocal::allocateSlot()
    {
        assert(std::this_thread::get_id() == mainThreadId_);
        assert(!shutdown_);   
        for (size_t i = 0; i < slots_.size(); i++) {
            if (slots_[i] == nullptr) {
                std::unique_ptr<SlotImpl> slot(new SlotImpl(*this, i));
                slots_[i] = slot.get();
                return std::move(slot);
            }
        }

        std::unique_ptr<SlotImpl> slot(new SlotImpl(*this, slots_.size()));
        slots_.push_back(slot.get());
        return std::move(slot);
    }

    void ThreadLocal::registerThread(Event::EventsLoop* loop, bool mainThread) 
    {
        if (mainThread) 
        {
            mainLoop_ = loop;
            threadLocalData_.loop_ = loop;
        } 
        else 
        {
            assert(!registeredThreads_.count(loop));
            registeredThreads_.insert(loop);
            loop->post([loop]
                {
                    threadLocalData_.loop_ = loop;
                }
            );
        }
    }
    
    void ThreadLocal::runOnAllThreads(Event::PostCb cb) 
    {
        assert(std::this_thread::get_id() == mainThreadId_);
        assert(!shutdown_);
        for (Event::EventsLoop* loop : registeredThreads_) 
        {
            loop->post(cb);
        }

        cb();
    }

    void ThreadLocal::runOnAllThreads(Event::PostCb cb, Event::PostCb all_threads_complete_cb) 
    {
        assert(std::this_thread::get_id() == mainThreadId_);
        assert(!shutdown_);

        std::shared_ptr<Event::PostCb> cb_guard(new Event::PostCb(cb),
            [this, all_threads_complete_cb](Event::PostCb* pcb)
            {
                mainLoop_->post(all_threads_complete_cb);
                delete pcb; 
            });

        for (Event::EventsLoop* loop : registeredThreads_) 
        {
            loop->post([cb_guard]{(*cb_guard)();});
        }

        cb();
    }

    void ThreadLocal::setThreadLocal(size_t index, ThreadLocalObjectSharedPtr object)
    {
        if (threadLocalData_.data_.size() <= index) 
        {
            threadLocalData_.data_.resize(index + 1);
        }

        threadLocalData_.data_[index] = object;
    }

    void ThreadLocal::shutdownGlobalThreading()
    {
        assert(std::this_thread::get_id() == mainThreadId_);
        assert(!shutdown_);
        shutdown_ = true;
    }

    void ThreadLocal::shutdownThread()
    {
        assert(shutdown_);
        for (auto it = threadLocalData_.data_.rbegin(); it != threadLocalData_.data_.rend(); ++it) 
        {
            it->reset();
        }
        threadLocalData_.data_.clear();
    }

    void ThreadLocal::SlotImpl::set(InitializeCb cb)
    {
        assert(std::this_thread::get_id() == parent_.mainThreadId_);
        assert(!parent_.shutdown_);

        for (Event::EventsLoop* loop : parent_.registeredThreads_) {
            const size_t index = index_;
            loop->post([index, cb, loop]() -> void { setThreadLocal(index, cb(loop)); });
        }

        // Handle main thread.
        setThreadLocal(index_, cb(parent_.mainLoop_));
    }

    void ThreadLocal::removeSlot(SlotImpl& slot)
    {
        assert(std::this_thread::get_id() == mainThreadId_);
        assert(!shutdown_);
        const uint64_t index = slot.index_;
        slots_[index] = nullptr;
        runOnAllThreads([index]() -> void {
            if (index < threadLocalData_.data_.size()) 
            {
                threadLocalData_.data_[index] = nullptr;
            }
        });
    }
}