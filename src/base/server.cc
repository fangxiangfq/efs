#include "server.h"
#include <assert.h>

namespace Server
{
    Server::Server(Event::EventsLoop* loop,
                  const std::string& nameArg) 
    :loop_(loop),
    name_(nameArg),
    threadPool_(new Thread::ThreadPool(loop, name_))
    {

    }

    void Server::setThreadNum(int numThreads)
    {
        assert(!started_);
        assert(0 <= numThreads);
        threadPool_->setThreadNum(numThreads);

        for(int i = 0; i < numThreads; ++i)
        {
            Event::LocalPtr local = std::make_unique<Event::Local>();
            Event::Event& masterEv = local->getMasterEv();
            Event::Event& workerEv = local->getMasterEv();
            masterEv.setLoop(loop_); 
            masterEv.setRead(std::bind(cb_, masterEv));
            workerEv.setRead(std::bind(cb_, workerEv));
            masterEv.enableRead();
            localArr_.push_back(std::move(local));
        }
    }

    void Server::start()
    {
        if (!started_)
        {
            started_ = true;
            threadPool_->start(localArr_, threadInitCallback_);
        }
    }

}