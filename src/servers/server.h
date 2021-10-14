#pragma once
#include "eventsloop.h"
#include "threadpool.h"
#include <atomic>
#include <string>
#include <vector>

//a base class sup http tcp udp etc
namespace Server
{
    using 
    class Server
    {
    public:
        enum class Option
        {
            kNoReusePort,
            kReusePort
        };

        Server(Event::EventsLoop* loop,
            const std::string& nameArg, const Event::TaskMap& taskmap);
        ~Server(){};
 
        void setThreadNum(int numThreads);
        const std::string& name() const { return name_; }
        Event::EventsLoop* getLoop() const { return loop_; }
        void setThreadInitCallback(const Thread::ThreadInitCallback& cb) { threadInitCallback_ = cb; }
        std::shared_ptr<Thread::ThreadPool> threadPool() { return threadPool_; }
        void start();
        void addEvent();

    protected:
        Event::EventsLoop* loop_;
        const std::string name_;
        Socket::SocketPairArr threadFds_;
        std::shared_ptr<Thread::ThreadPool> threadPool_;
        Thread::ThreadInitCallback threadInitCallback_;
        std::atomic<bool> started_{false};

    };  
}