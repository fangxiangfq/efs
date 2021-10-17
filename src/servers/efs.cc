#include "efs.h"


void Efs::registerWorkerTasks(const Event::TaskMap& taskmap) 
{
    assert(!started_);
    workerTaskMap_ = taskmap;
}

void Efs::registerWorkerTasks(const short& task, const Event::TaskCb& cb) 
{
    assert(!started_);
    workerTaskMap_.emplace(task, cb);
}

void Efs::registerMasterTasks(const Event::TaskMap& taskmap) 
{
    assert(!started_);
    masterTaskMap_ = taskmap;
}

void Efs::registerMasterTasks(const short& task, const Event::TaskCb& cb) 
{
    assert(!started_);
    masterTaskMap_.emplace(task, cb);
}

void Efs::dispatch(Event::Event& ev) 
{
    if(ev.ownerLoop() != &main_loop_)
    {
        //todo log
        abort();
    }

    switch (ev.type())
    {
    case Event::EvType::udp:
        onUdpMessage(ev);
        break;
    case Event::EvType::local:
        onLocalMessage(ev);
        break;
    default:
        break;
    }
}

void Efs::taskPost(Buffer::Buffer& buf) 
{
    int savedErrno = 0;
    int workerFd = server_.getNextWorkerFd();
    assert(workerFd > 0);
    if(0 > buf.send(workerFd, &savedErrno))
    {
        //todo log
    }
}

void Efs::onUdpMessage(Event::Event& ev) 
{
    Buffer::Buffer buf;
    int fd = ev.fd();
    uint16_t dstnum = static_cast<uint16_t>(routeMap_.count(fd));
    uint16_t task = static_cast<uint16_t>(Task::udpforward);
    buf.write(task);
    buf.write(fd);
    buf.write(dstnum);
    for(auto it = routeMap_.lower_bound(fd); it != routeMap_.upper_bound(fd); ++it)
    {
        buf.write(it->second);
    }
    //todo disable fd
    ev.disableAll();
    taskPost(buf);
}

void Efs::onLocalMessage(Event::Event& ev) 
{
     const Event::Event& ev
}


