#pragma once

#include <memory>
#include <map>
#include <set>
#include <vector>
#include <string>
#include "server.h"   
#include "buffer.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

class Efs
{
public:
    enum class Task
    { 
        udpforward,
        udpforwardack
    };

    Efs();


    virtual ~Efs()=default;
    virtual void start(){};

    void registerTasks(Event::TaskMap& taskmap);
    void registerTasks(const short& task, const Event::TaskCb& cb);

protected:
    virtual void dispatch(Event::Event& ev);
    virtual void taskPost(Buffer::Buffer& buf);
    virtual void taskExec(){};
    
    virtual void onTcpLink(Event::Event& ev){};
    virtual void onTcpMessage(Event::Event& ev){};
    virtual void onUdpMessage(Event::Event& ev); 
    virtual void onLocalMessage(Event::Event& ev);
    virtual void setTaskHeader(Buffer::Buffer& buf, size_t len);
    virtual bool checkTaskHeader(Buffer::Buffer& buf, uint16_t& task);
private:
    bool started_;
    Event::EventsLoop main_loop_;
    Server::Server server_;

    Event::TaskMap taskMap_;
    std::set<uint16_t> portSet_;
    std::map<uint16_t, int> portFdMap_;
    std::multimap<int, int> routeMap_;
};