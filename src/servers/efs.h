#pragma once

#include <memory>
#include <map>
#include <set>
#include <vector>
#include "server.h"   
    
class Efs
{
public:
    enum class Task
    { 
        udpforward,
        udpforwardack
    };

    Efs(){};
    virtual ~Efs()=default;
    virtual void start(){};

    void registerWorkerTasks(const Event::TaskMap& taskmap);
    void registerWorkerTasks(const short& task, const Event::TaskCb& cb);
    void registerMasterTasks(const Event::TaskMap& taskmap);
    void registerMasterTasks(const short& task, const Event::TaskCb& cb);

protected:
    virtual void dispatch(const Event::Event& ev);
    virtual void taskPost();
    virtual void taskExec();
    
    virtual void onConnect(){};//tcplink
    virtual void onConnect(){};//tcpmsg
    virtual void onUdpMessage(const int& fd, const uint16_t& port); 
private:
    bool started_;
    Event::EventsLoop main_loop_;
    Server::Server server_;

    Event::TaskMap masterTaskMap_;
    Event::TaskMap workerTaskMap_;
    std::set<uint16_t> portSet_;
    std::map<uint16_t, int> portFdMap_;
    std::multimap<int, int> routeMap_;
};