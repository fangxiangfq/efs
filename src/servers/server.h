#pragma once
#include "eventsloop.h"
#include "threadpool.h"
#include <atomic>
#include <string>
//a base class sup http tcp udp etc
namespace Server
{
    class Server
    {
    public:
        enum class Type
        {
            udp,
            tcp
        };

        enum class Option
        {
            kNoReusePort,
            kReusePort
        };

        Server(Event::EventsLoop* loop,
            const std::string& nameArg,
            Option option = Option::kReusePort);
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
        std::shared_ptr<Thread::ThreadPool> threadPool_;
        Thread::ThreadInitCallback threadInitCallback_;
        std::atomic<bool> started_{false};

    };  
}