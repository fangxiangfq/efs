#include "server.h"
#include <assert.h>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

#define MTU_SIZE 1500
namespace Server
{
    Server::Server(Event::EventsLoop* loop,
                  const std::string& nameArg, uint16_t threadNum) 
    :loop_(loop),
    name_(nameArg),
    workerFactory_(new Thread::WorkerFactory(loop, name_, threadNum))
    {
        setLogLevel(trace);
        init(); 
        STD_DEBUG("server construct ok, threadnum {}", threadNum);
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
        Rest::JsonBuilder::registerMsg(Rest::Code::terno_exist, "terno exsit");
        Rest::JsonBuilder::registerMsg(Rest::Code::terno_not_exist, "terno not exsit");

        if(!loop_)
        {
            STD_CRIT("server[{}] init without valid loop", name_);
            return;
        }

        if(!evManager_.restManager_)
        {
            evManager_.restManager_.reset(new Event::TcpListenEvent(std::bind(&Server::onHttpConnect, this, _1), port, true, loop_));
        }

        evManager_.restManager_->enableRead();

        STD_INFO("start http listening on 0.0.0.0:{}", port);
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
                    Rest::setHttpResponse(rsp, code);
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
        Rest::setHttpResponse(rsp, code);
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

        if(evManager_.termManager_.count(terno) != 0)
        {
            code = Rest::Code::terno_exist;
            return 0;
        }

        auto it = portManager_.begin();
        uint16_t port = *it;
        portManager_.erase(it);

        evManager_.termManager_.emplace(terno, std::move(EvManager::createUdpEvPtr(port, peerPort, peerIp, loop_)));
        evManager_.termManager_[terno]->setMsgCb(std::bind(&Server::onUdpMessage, this, _1));
        evManager_.termManager_[terno]->enableRead();
        return port;
    }
    
    void Server::onDelete(const std::string& terno, Rest::Code& code) 
    {
        auto it = evManager_.termManager_.find(terno);
        if(it != evManager_.termManager_.end())
        {
            it->second->disableAll();
            it->second->remove();
            portManager_.emplace(it->second->sock().localAddr_.port());
            evManager_.termManager_.erase(it);
        }
    }
    
    void Server::onRoute(const std::string& terno, const std::vector<std::string>& dst, Rest::Code& code) 
    {
        auto it = evManager_.termManager_.find(terno);
        if(it != evManager_.termManager_.end())
        {
            for(auto& str : dst)
            {
                auto dt = evManager_.termManager_.find(str);
                if(dt != evManager_.termManager_.end())
                {
                    routeManager_.add(it->second->sock(), dt->second->sock());
                }
            }
            return;
        }
        code = Rest::Code::terno_not_exist;
    }

    void Server::onUdpMessage(const Event::UdpEvPtr& ev) // make sure mem safe if ev removed during forward 
    {
        auto it = routeManager_.dstmap().find(ev->sock());
        if(it == routeManager_.dstmap().end() || it->second.size() < maxForwardPerLoop_) //fullforward include no dst
        {
            Event::EventsLoop* masterLoop = loop_;
            //cpy value for thread safe
            masterLoop->disableEvent(*ev);
            Event::UdpEvPtr evPtr = ev;
            std::shared_ptr<Route::RouteSet> setPtr;
            if(it != routeManager_.dstmap().end())
                setPtr.reset(new Route::RouteSet(it->second));
            Thread::TaskCbPtr TaskPtr(new Thread::TaskCb([setPtr, evPtr, masterLoop](){
                auto& src = evPtr->sock();
                Buffer::Buffer buf(MTU_SIZE);
                int saveErrno = 0;
                ssize_t n = buf.recv(src.sockfd_, &saveErrno, MTU_SIZE);
                if(n <= 0){
                    STD_ERROR("read error ret[{}] errno[{}] fd[{}] port[{}]", n, saveErrno, src.sockfd_, src.localAddr_.port()); //do not close
                    masterLoop->enableEvent(*evPtr);
                    return;
                }

                if(!setPtr){
                    masterLoop->enableEvent(*evPtr); 
                    return;
                }

                for(auto &dst : *setPtr){
                    n = buf.sendto(dst, &saveErrno);
                    if(n <= 0){
                       STD_ERROR("send error ret[{}] errno[{}] fd[{}] port[{}]", n, saveErrno, dst.sockfd_, dst.localAddr_.port());//do not close
                    }
                }
                masterLoop->enableEvent(*evPtr); 
            }));
            workerFactory_->postTask(TaskPtr);
        }
        else//live mode
        {
            auto& src = ev->sock();
            int saveErrno = 0;
            std::shared_ptr<Buffer::Buffer> buf(new Buffer::Buffer(MTU_SIZE));
            ssize_t n = buf->recv(src.sockfd_, &saveErrno, MTU_SIZE);
            if(n <= 0){
                STD_ERROR("read error ret[{}] errno[{}] fd[{}] port[{}]", n, saveErrno, src.sockfd_, src.localAddr_.port()); //do not close
                return;
            }

            uint16_t threadNum = workerFactory_->getThreadNum();
            if(!threadNum){//no worker
                for(auto &dst : it->second){
                    n = buf->sendto(dst, &saveErrno);
                    if(n <= 0){
                        STD_ERROR("send error ret[{}] errno[{}] fd[{}] port[{}]", n, saveErrno, dst.sockfd_, dst.localAddr_.port());//do not close
                    }
                }
                return;
            }

            size_t interval = it->second.size() / threadNum;
            std::shared_ptr<Route::RouteSet> setPtr(new Route::RouteSet(it->second));
            for(uint16_t i = 0; i < threadNum; ++i){
                Thread::TaskCbPtr TaskPtr(new Thread::TaskCb([buf, setPtr, i, interval](){
                    size_t count = 0;
                    for(auto& dst : *setPtr){ //todo 
                        if(count >= i * interval && count < (i+1)* interval){
                            int saveErr = 0;
                            ssize_t len = buf->sendto(dst, &saveErr);
                            if(len <= 0){
                                STD_ERROR("send error ret[{}] errno[{}] fd[{}] port[{}]", len, saveErr, dst.sockfd_, dst.localAddr_.port());//do not close
                            }
                        }
                        count++;
                    }
                }));

                workerFactory_->postTask(TaskPtr);
            }
        }
    }

    void Server::postTask(Thread::TaskCbPtr cb) 
    {
        if(workerFactory_->getThreadNum() == 0){
            if(cb)
                (*cb)();
            return;
        }

        Thread::TaskData* task = new Thread::TaskData(cb);

        auto workerev = workerFactory_->getNextWorkerEv();

        workerev->write(task);
    }

    void Server::execTask(void* taskdata)
    {
        Thread::TaskData* task = static_cast<Thread::TaskData*>(taskdata);
        STD_CRIT("read data pointer {}", static_cast<void*>(task));
        if(task->cb_)
            (*task->cb_)();
        task->cb_ = nullptr;
        delete task;
    }
}