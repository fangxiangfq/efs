#pragma once

#include <memory>
#include <map>
#include <set>
#include <vector>
#include <string>
#include "server.h"   
#include "buffer.h"

class Efs
{
public:
    enum class Task
    { 
        udpforward,
        udpforwardack
    };

    Efs() 
    :main_loop_(-1, masterTaskMap_), server_(&main_loop_, std::string("efs"), workerTaskMap_){}

    virtual ~Efs()=default;
    virtual void start(){};

    void registerWorkerTasks(const Event::TaskMap& taskmap);
    void registerWorkerTasks(const short& task, const Event::TaskCb& cb);
    void registerMasterTasks(const Event::TaskMap& taskmap);
    void registerMasterTasks(const short& task, const Event::TaskCb& cb);

protected:
    virtual void dispatch(const Event::Event& ev);
    virtual void taskPost(Buffer::Buffer& buf);
    virtual void taskExec(){};
    
    virtual void onTcpLink(const Event::Event& ev){};
    virtual void onTcpMessage(Event::Event& ev){};
    virtual void onUdpMessage(Event::Event& ev); 
    virtual void onLocalMessage(const Event::Event& ev);
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