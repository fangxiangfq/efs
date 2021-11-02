#include "server.h"
#include <assert.h>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

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
        restManagerInit();
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
            portManager_.emplace(i); 
        }
    }
    
    void Server::restManagerInit(uint16_t port) 
    {
        Rest::JsonBuilder::registerMsg(Rest::Code::success, "success");
        Rest::JsonBuilder::registerMsg(Rest::Code::bad_request, "bad request");
        Rest::JsonBuilder::registerMsg(Rest::Code::server_error, "server error");
        Rest::JsonBuilder::registerMsg(Rest::Code::server_full, "server full");
        Rest::JsonBuilder::registerMsg(Rest::Code::unknown_url, "unknown url");

        if(!loop_)
        {
            //todo log
            return;
        }

        if(!evManager_.restManager_)
        {
            evManager_.restManager_.reset(new Event::TcpListenEvent(std::bind(&Server::onHttpConnect, this, _1), port, true, loop_));
        }

        evManager_.restManager_->enableRead();
    }
    
    void Server::onHttpConnect(Event::HttpEvPtr httpev) 
    {
        httpev->setLoop(loop_);
        httpev->setReqCb(std::bind(&Server::onRequest, this, _1, _2));
        httpev->setCloseCb(std::bind(&Server::onHttpDisconnect, this, _1));
        httpev->enableRead();
        evManager_.httpConManager_.emplace(std::move(httpev));
    }

    void Server::onHttpDisconnect(const Event::HttpEvPtr& ev)
    {
        evManager_.httpConManager_.erase(ev);
        ev->onClose();
        if(ev.use_count() > 1)
        {
            STD_ERROR("ptr count = {}", ev.use_count());
        }
    }

    void Server::onRequest(const Http::HttpRequest& req, Http::HttpResponse& rsp)
    {
        Rest::JsonParser parser(req.body());
        std::string terno;
        Rest::Code code = Rest::Code::success;

        if (req.path() == "/create")
        {
            std::string ip;
            uint16_t peer_port;
            if(!parser.createParse(terno, ip, peer_port))
            {
                code = Rest::Code::bad_request;
            }
            else
            {
                uint16_t port = onCreate(terno, ip, peer_port, code);
                if(0 != port)
                {
                    Rest::JsonBuilder builder(code, "port", port);
                    rsp.setBody(builder.toString());
                    return;
                }
            }
        }
        else if(req.path() == "/delete")
        {
            if(!parser.deleteParse(terno))
            {
                code = Rest::Code::bad_request;
            }
            else
            {
                onDelete(terno, code);
            }
        }
        else if(req.path() == "/route")
        {
            std::vector<std::string> dst;
            if(!parser.routeParse(terno, dst))
            {
                code = Rest::Code::bad_request;
            }
            else
            {
                onRoute(terno, dst, code);
            }
        }
        else
        {
            code = Rest::Code::unknown_url;
        }
        
        Rest::JsonBuilder builder(code);
        rsp.setBody(builder.toString());
    }

    uint16_t Server::onCreate(const std::string& terno, const std::string& peerIp, const uint16_t& peerPort, Rest::Code& code)
    {
        if(portManager_.empty())
        {
            code = Rest::Code::server_full;
            return 0;
        }

        auto it = portManager_.begin();
        uint16_t port = *it;
        portManager_.erase(it);
        return port;
    }
    
    void Server::onDelete(const std::string& terno, Rest::Code& code) 
    {
        
    }
}