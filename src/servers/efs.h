#pragma once

#include <memory>
#include <map>
#include <set>
#include <vector>
#include "server.h"   
    
class Efs
{    
    Efs(){};
    void start(){};

    void registerWorkerTasks(Event::TaskMap taskmap)
    {
        assert(!started_);
        workerTaskMap_ = taskmap;
    }

    void registerWorkerTasks(short task, Event::TaskCb cb)
    {
        assert(!started_);
        workerTaskMap_.emplace(task, cb);
    }

    void registerMasterTasks(Event::TaskMap taskmap)
    {
        assert(!started_);
        masterTaskMap_ = taskmap;
    }

    void registerMasterTasks(short task, Event::TaskCb cb)
    {
        assert(!started_);
        masterTaskMap_.emplace(task, cb);
    }

private:
    bool started_;
    Event::EventsLoop main_loop_;
    Server::Server server_;

    Event::TaskMap masterTaskMap_;
    Event::TaskMap workerTaskMap_;
    std::set<uint16_t> portSet_;
    std::map<std::string, uint16_t> termMap_;
    std::multimap<uint16_t, uint16_t> routeMap_;
};