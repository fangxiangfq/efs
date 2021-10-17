#pragma once
#include <functional>
#include <string>
#include <vector>
#include "thread.h"
#include "eventsloop.h"

namespace Thread
{
    using ThreadInitCallback = std::function<void(Event::EventsLoop*)>;
    class ThreadPool
    {
    public:
        ThreadPool(Event::EventsLoop* baseLoop, const std::string& nameArg, const Event::TaskMap& taskmap);
        ~ThreadPool();
        void setThreadNum(int numThreads) { numThreads_ = numThreads; }
        void start(const Socket::SocketPairArr& threadFds, const ThreadInitCallback& cb = ThreadInitCallback());

        Event::EventsLoop* getNextLoop();

        Event::EventsLoop* getLoopForHash(size_t hashCode);

        int getNextLoopIndex();

        std::vector<Event::EventsLoop*> getAllLoops();

        bool started() const
        { return started_; }

        const std::string& name() const
        { return name_; }

    private:

        Event::EventsLoop* baseLoop_;
        std::string name_;
        bool started_;
        int numThreads_;
        const Event::TaskMap& taskmap_;
        
        size_t next_;
        std::vector<std::unique_ptr<Thread>> threads_;
        std::vector<Event::EventsLoop*> loops_;
    };
}
