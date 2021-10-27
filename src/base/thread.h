#pragma once
#include <functional>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "eventsloop.h"


namespace Thread
{
    using ThreadPtr = std::unique_ptr<std::thread>;
    class Thread
    {
    public:
        using ThreadFunc = std::function<void ()>;
        using ThreadInitCallback = std::function<void(Event::EventsLoop*)>;

        Thread(Event::Event& workerEv, const ThreadInitCallback& cb = ThreadInitCallback(), const std::string& name = std::string());
        ~Thread();
        Thread(const Thread&) = delete;
	    Thread& operator=(const Thread&) = delete;
        Event::EventsLoop* startLoop();
    private:
        Event::EventsLoop* loop_;
        void threadFunc();
        bool exiting_;
        ThreadPtr thread_;
        std::mutex mutex_;
        std::condition_variable cond_;
        ThreadInitCallback callback_;
        const std::string name_;
        Event::Event& workerEv_;
    };
}