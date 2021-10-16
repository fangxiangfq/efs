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

void Efs::dispatch(const Event::Event& ev) 
{
    if(ev.ownerLoop() != &main_loop_)
    {
        //todo log
        abort();
    }

    switch (ev.type())
    {
    case Event::EvType::udp:
        /* code */
        onUdpMessage(ev.fd());
        break;
    
    default:
        break;
    }
}

void Efs::onUdpMessage(const int& fd) 
{
    if(0 == RouteMap_.count(fd))
    {
        //todo log
    }
    
    
}
