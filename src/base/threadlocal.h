#pragma once

#include "eventsloop.h"
#include <thread>
#include <vector>
#include <set>
#include <assert.h>

namespace ThreadLocal
{   
    class ThreadLocalObject
    {
    public:
        virtual ~ThreadLocalObject() = default;
        template <class T> T& asType() 
        {
            assert(dynamic_cast<T*>(this) != nullptr);
            return *static_cast<T*>(this);
        }
    };

    using ThreadLocalObjectSharedPtr = std::shared_ptr<ThreadLocalObject>;
    using InitializeCb = std::function<ThreadLocalObjectSharedPtr(Event::EventsLoop* loop)>;
    using UpdateCb = std::function<void(ThreadLocalObjectSharedPtr)>;
    
    class Slot 
    {
    public:
        virtual ~Slot() {}
        virtual ThreadLocalObjectSharedPtr get()=0;
        template <class T> T& getTyped() { return *std::dynamic_pointer_cast<T>(get()); }
        virtual void runOnAllThreads(Event::PostCb cb)=0;
        virtual void runOnAllThreads(Event::PostCb cb, Event::PostCb all_threads_complete_cb)=0;
        virtual void set(InitializeCb cb)=0;
    };

    using SlotPtr = std::unique_ptr<Slot>;

    class ThreadLocal
    {
    public:
        ThreadLocal() : mainThreadId_(std::this_thread::get_id()) {}
        ~ThreadLocal();
        SlotPtr allocateSlot();
        void registerThread(Event::EventsLoop* loop, bool mainThread);
        void shutdownGlobalThreading();
        void shutdownThread();
        Event::EventsLoop* eventsLoop();

        void runOnAllThreads(Event::PostCb cb);
        void runOnAllThreads(Event::PostCb cb, Event::PostCb main_callback);
        static void setThreadLocal(uint32_t index, ThreadLocalObjectSharedPtr object);

    private:
        struct SlotImpl : public Slot 
        {
            SlotImpl(ThreadLocal& parent, uint32_t index) : parent_(parent), index_(index) {}
            ~SlotImpl() { parent_.removeSlot(*this); }

            // ThreadLocal::Slot
            ThreadLocalObjectSharedPtr get() override
            {
                assert(threadLocalData_.data_.size() > index_);
                return threadLocalData_.data_[index_];
            }
            void runOnAllThreads(Event::PostCb cb) override 
            { 
                parent_.runOnAllThreads(cb);
            }

            void runOnAllThreads(Event::PostCb cb, Event::PostCb main_callback) override 
            {
                parent_.runOnAllThreads(cb, main_callback);
            }

            void set(InitializeCb cb) override;

            ThreadLocal& parent_;
            const uint32_t index_;
        };

        struct ThreadLocalData 
        {
            Event::EventsLoop* loop_{};
            std::vector<ThreadLocalObjectSharedPtr> data_;
        };
        
        void removeSlot(SlotImpl& slot);

        static thread_local ThreadLocalData threadLocalData_;
        std::vector<SlotImpl*> slots_;
        std::set<Event::EventsLoop*> registeredThreads_;
        std::thread::id mainThreadId_;
        Event::EventsLoop* mainLoop_{};
        std::atomic<bool> shutdown_{};
    };
}