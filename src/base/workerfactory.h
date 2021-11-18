#pragma once
#include <functional>
#include <string>
#include <vector>
#include "thread.h"
#include "eventsloop.h"
#include "evmanager.h"
#include "logger.h"

namespace Thread
{
    using WorkInitCb = std::function<void(Event::EventsLoop*)>;
    using TaskCb = std::function<void()>;
    using TaskCbPtr = std::shared_ptr<TaskCb>;

    struct TaskData
    {
        TaskData(const TaskCbPtr& cb):cb_(cb){};
        TaskCbPtr cb_; //used for same task in multi thread
    };

    class WorkerFactory
    {
    public:
        WorkerFactory(Event::EventsLoop* baseLoop, const std::string& nameArg, uint16_t threadNum = 8);
        ~WorkerFactory();
        void start(const WorkInitCb& cb = WorkInitCb());
        uint16_t getThreadNum() const { return threadNum_; }
        uint16_t getNextIdx(); 
        Event::TaskEvPtr getNextWorkerEv();
        Event::SockPairPtr getNextWorkerSockEv();  
        Event::EventsLoop* getNextLoop();
        Event::EventsLoop* getLoopForHash(size_t hashCode);
        
        bool started() const { return started_; }
        const std::string& name() const { return name_; }

        void postTask(const TaskCbPtr& cb);
    private:
        void init();
        static void execTask(void* taskdata);

        Event::TaskMsgCb cb_;
        Event::EventsLoop* baseLoop_;
        std::string name_;
        bool started_;
        uint16_t threadNum_;
        uint16_t workerIdx_;
        std::map<uint16_t, TaskCb> taskMap_;
        std::vector<Event::TaskEvPtr> workertaskEv_; //not used
        std::vector<Event::TaskEvPtr> mastertaskEv_; //not used
        std::vector<std::unique_ptr<Thread>> threads_;
        std::vector<Event::EventsLoop*> loops_;
        std::vector<Event::SockPairPtrPair> taskSockPairEv_;
    };
}
