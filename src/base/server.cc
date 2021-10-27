#include "server.h"
#include "rest.h"
#include <assert.h>

namespace Server
{
    Server::Server(Event::EventsLoop* loop,
                  const std::string& nameArg, uint16_t threadNum) 
    :loop_(loop),
    name_(nameArg),
    workerFactory_(new Thread::WorkerFactory(loop, name_, threadNum))
    {
    }
    
    void Server::init() 
    {
        Rest::JsonBuilder::registerMsg(Rest::Code::success, "success");
        Rest::JsonBuilder::registerMsg(Rest::Code::bad_request, "bad request");
        Rest::JsonBuilder::registerMsg(Rest::Code::server_error, "server error");
        Rest::JsonBuilder::registerMsg(Rest::Code::server_full, "server full");
    }

    void Server::start()
    {
        if (!started_)
        {
            started_ = true;
        }

        workerFactory_->start();
    }
}