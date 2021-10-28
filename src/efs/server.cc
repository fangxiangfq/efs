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
        init(); 
    }
    
    void Server::init() 
    {
        portManagerInit();
        restInit();
    }

    void Server::start()
    {
        if (!started_)
        {
            started_ = true;
        }

        workerFactory_->start();
    }
    
    void Server::portManagerInit(uint16_t min_media_port, uint16_t max_media_port) 
    {
        assert(!started_);
        
        for(uint16_t i = min_media_port; i <= max_media_port; i = static_cast<uint16_t>(i + 2))
        {
            portManager_.insert(i); 
        }
    }
    
    void Server::restInit(uint16_t port) 
    {
        Rest::JsonBuilder::registerMsg(Rest::Code::success, "success");
        Rest::JsonBuilder::registerMsg(Rest::Code::bad_request, "bad request");
        Rest::JsonBuilder::registerMsg(Rest::Code::server_error, "server error");
        Rest::JsonBuilder::registerMsg(Rest::Code::server_full, "server full");

        if(!loop_)
        {
            //todo log
            return;
        }

        if(!restEv_)
            restEv_ = std::make_unique<Event::TcpListenEvent>(port, loop_);
    }
}