#pragma once
#include <atomic>
#include <string>
#include <vector>
#include "eventsloop.h"
#include "threadpool.h"
#include "chan.h"
#include "threadlocal.h"
//a base class sup http tcp udp etc
namespace Server
{
    class Server
    {
    public:
        enum class Option
        {
            kNoReusePort,
            kReusePort
        };

        Server(Event::EventsLoop* loop, const std::string& nameArg);
        ~Server(){};

        void setEvCallback(const Event::EventCallbackEx& cb) { cb_ = cb; }
        void setThreadNum(int numThreads);
        const std::string& name() const { return name_; }
        Event::EventsLoop* getLoop() const { return loop_; }
        void setThreadInitCallback(const Thread::ThreadInitCallback& cb) { threadInitCallback_ = cb; }
        std::shared_ptr<Thread::ThreadPool> threadPool() { return threadPool_; }
        void start();
        void createChan();
        void distoryChan();

        int getNextWorkerFd()
        {
            int idx = threadPool_->getNextLoopIndex();
            if(idx < 0 || static_cast<size_t>(idx) > localArr_.size())
                return -1;
            return localArr_[idx]->getSockp().first();
        } 

    protected:
        Event::EventsLoop* loop_;
        const std::string name_;
        
        std::shared_ptr<Thread::ThreadPool> threadPool_;
        Thread::ThreadInitCallback threadInitCallback_;
        std::atomic<bool> started_{false};

        Event::EventCallbackEx cb_;
        Event::LocalsArr      localArr_;
        Event::ChansMap       chansMap_;
    };  
}