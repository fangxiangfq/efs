#include "server.h"
#include <assert.h>

namespace Server
{
    Server::Server(Event::EventsLoop* loop,
                  const std::string& nameArg,
                  Option option) 
    :loop_(loop),
    name_(nameArg),
    threadPool_(new Thread::ThreadPool(loop, name_))
    {

    }

    void Server::setThreadNum(int numThreads)
    {
        assert(0 <= numThreads);
        threadPool_->setThreadNum(numThreads);
    }

    void Server::start()
    {
        if (started_)
        {
            started_ = true;
            threadPool_->start(threadInitCallback_);
        }
    }
}