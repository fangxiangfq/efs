#pragma once
#include <functional>
#include <string>
#include <vector>
#include "thread.h"
#include "eventsloop.h"
#include "evmanager.h"

namespace Thread
{
    using WorkInitCb = std::function<void(Event::EventsLoop*)>;

    class WorkerFactory
    {
    public:
        using TaskCb = std::function<void()>;

        struct TaskData
        {
            uint16_t taskNo;
            TaskCb Cb;
        };

        void postTask(){};
        void postEvent(Event::Event& ev){};
        void execTask(){};

        WorkerFactory(Event::EventsLoop* baseLoop, const std::string& nameArg, uint16_t threadNum = 8);
        ~WorkerFactory();
        void init();
        void start(const WorkInitCb& cb = WorkInitCb());
        uint16_t threadNum() const { return threadNum_; };
        Event::EventsLoop* getNextLoop();
        Event::EventsLoop* getLoopForHash(size_t hashCode);

        bool started() const { return started_; }
        const std::string& name() const { return name_; }
    private:
        Event::EventsLoop* baseLoop_;
        std::string name_;
        bool started_;
        uint16_t threadNum_;
        uint16_t workerIdx;

        std::map<uint16_t, TaskCb> taskMap_;
        std::vector<Event::TaskEvPtr> workertaskEv_;
        std::vector<Event::TaskEvPtr> mastertaskEv_;
        std::vector<std::unique_ptr<Thread>> threads_;
        std::vector<Event::EventsLoop*> loops_;
    };
}
