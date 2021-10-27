#pragma once
#include <atomic>
#include <string>
#include <vector>
#include "eventsloop.h"
#include "chan.h"
#include "workerfactory.h"

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

        Server(Event::EventsLoop* loop, const std::string& nameArg = "efs", uint16_t threadNum = 8);
        ~Server(){};
        void init();
        void setEvCallback(const Event::EventCallbackEx& cb) { cb_ = cb; }
        const std::string& name() const { return name_; }
        Event::EventsLoop* getLoop() const { return loop_; }
        void start();

    protected:
        Event::EventsLoop* loop_;
        const std::string name_;
        std::unique_ptr<Thread::WorkerFactory> workerFactory_{};
        std::atomic<bool> started_{false};
        Event::EventCallbackEx cb_;
    };  
}