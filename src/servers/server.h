#pragma once
#include <atomic>
#include <string>
#include <vector>
#include "eventsloop.h"
#include "threadpool.h"
#include "chan.h"

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

        Server(Event::EventsLoop* loop, const std::string& nameArg, const Event::TaskMap& taskmap);
        ~Server(){};
 
        void setThreadNum(int numThreads);
        const std::string& name() const { return name_; }
        Event::EventsLoop* getLoop() const { return loop_; }
        void setThreadInitCallback(const Thread::ThreadInitCallback& cb) { threadInitCallback_ = cb; }
        std::shared_ptr<Thread::ThreadPool> threadPool() { return threadPool_; }
        void start();
        void CreateChan(){};
        void DistoryChan(){};

        int getNextWorkerFd()
        {
            int idx = threadPool_->getNextLoopIndex();
            if(idx < 0 || static_cast<size_t>(idx) > threadFds_.size())
                return -1;
            return threadFds_[idx]->get_first();
        } 

    protected:
        Event::EventsLoop* loop_;
        const std::string name_;
        
        std::shared_ptr<Thread::ThreadPool> threadPool_;
        Thread::ThreadInitCallback threadInitCallback_;
        std::atomic<bool> started_{false};

        using AckEvPtr = std::unique_ptr<Event::Event>;
        using AckEvArr = std::vector<AckEvPtr>;

        Socket::SocketPairArr threadFds_;
        AckEvArr              ackEvs_;
        Event::ChansMap       chansMap_;
    };  
}