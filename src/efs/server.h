#pragma once

#include <atomic>
#include <string>
#include <vector>
#include <set>
#include "eventsloop.h"
#include "workerfactory.h"
#include "rest.h"
#include "logger.h"
#include "http.h"
#include "router.h"

namespace Server
{
    class Server
    {
    public:
        enum class Option
        {
            kNoReusePort,
            kReusePort
        };

        Server(Event::EventsLoop* loop, const std::string& nameArg = "efs", uint16_t threadNum = 7);
        ~Server(){};
        void init();
        const std::string& name() const { return name_; }
        Event::EventsLoop* getLoop() const { return loop_; }
        void start();
        void setLogLevel(level_enum lv) { logger_.setLogLv(lv); }
        
    private:
        Logger logger_;
        Event::EventsLoop* loop_;
        const std::string name_;
        std::unique_ptr<Thread::WorkerFactory> workerFactory_;
        std::atomic<bool> started_{false};
    //init
        void portManagerInit(uint16_t min_media_port = 10000, uint16_t max_media_port = 30000);
        void restManagerInit(uint16_t port = 8000);
    //ioManager
        std::set<uint16_t> portManager_;
        Event::EventManager evManager_;  
        Route::Router routeManager_;
        const size_t maxForwardPerLoop_{200};
    //ioCallBack
        void postTask(Thread::TaskCbPtr cb); //not used
        void execTask(void* taskdata); //not used
        void onHttpConnect(Event::HttpEvPtr httpev);
        void onRequest(const Http::HttpRequest& req, Http::HttpResponse& rsp);
        void onHttpDisconnect(const Event::HttpEvPtr& ev);
        void onUdpMessage(const Event::UdpEvPtr& ev);
    //restManager
        uint16_t onCreate(const std::string& terno, const std::string& peerIp, const uint16_t& peerPort, Rest::Code& code);
        void onDelete(const std::string& terno, Rest::Code& code);
        void onRoute(const std::string& terno, const std::vector<std::string>& dst, Rest::Code& code);
    };  
}