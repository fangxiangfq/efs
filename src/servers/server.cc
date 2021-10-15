#include "server.h"
#include <assert.h>

namespace Server
{
    Server::Server(Event::EventsLoop* loop,
                  const std::string& nameArg, const Event::TaskMap& taskmap) 
    :loop_(loop),
    name_(nameArg),
    threadPool_(new Thread::ThreadPool(loop, name_, taskmap))
    {

    }

    void Server::setThreadNum(int numThreads)
    {
        assert(!started_);
        assert(0 <= numThreads);
        threadPool_->setThreadNum(numThreads);
        for(int i = 0; i < numThreads; ++i)
        {
            Socket::SocketPtr sockpair =  std::make_unique<Socket::SocketPair>();
            threadFds_.push_back(std::move(sockpair));
        }
    }

    void Server::start()
    {
        if (!started_)
        {
            started_ = true;
            threadPool_->start(threadFds_, threadInitCallback_);
        }
    }

}