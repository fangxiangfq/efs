#pragma once
#include "eventsloop.h"
#include <thread>
#include <set>

namespace ThreadLocal
{

    class ThreadLocalObject
    {
    public:
        virtual ~ThreadLocalObject() = default;
        virtual void shutdown() = 0;

        template <class T> T& asType() 
        {
            assert(dynamic_cast<T*>(this) != nullptr);
            return *static_cast<T*>(this);
        }
    };

    using ThreadLocalObjectSharedPtr = std::shared_ptr<ThreadLocalObject>;
    using InitializeCb = std::function<ThreadLocalObjectSharedPtr(Event::EventsLoop* loop)>;
    using UpdateCb = std::function<void(ThreadLocalObjectSharedPtr)>;
    using PostCb = std::function<void()>;
    
    class ThreadLocal
    {
    public:
        ThreadLocal() : mainThreadId_(std::this_thread::get_id()) {}
        ~ThreadLocal(){ reset(); }
        uint32_t allocateSlot(){ return nextSlotId_++; }
        ThreadLocalObjectSharedPtr get(uint32_t index);
        void registerThread(Event::EventsLoop* loop, bool mainThread);
        void runOnAllThreads(PostCb cb);
        void set(uint32_t index, InitializeCb cb);
        void shutdownThread();

    private:
        struct ThreadLocalData 
        {
            std::map<uint32_t, ThreadLocalObjectSharedPtr> data_;
        };

        void reset();
        static std::atomic<uint32_t> nextSlotId_;
        static thread_local ThreadLocalData threadLocalData_;
        static std::set<Event::EventsLoop*> registeredThreads_;
        std::thread::id mainThreadId_;
        Event::EventsLoop* mainLoop_{};
    };
}