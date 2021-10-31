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

        Server(Event::EventsLoop* loop, const std::string& nameArg = "efs", uint16_t threadNum = 8);
        ~Server(){};
        void init();
        void setEvCallback(const Event::EventCallbackEx& cb) { cb_ = cb; }
        const std::string& name() const { return name_; }
        Event::EventsLoop* getLoop() const { return loop_; }
        void start();
        
    private:
        Logger logger_;
        Event::EventsLoop* loop_;
        const std::string name_;
        std::unique_ptr<Thread::WorkerFactory> workerFactory_;
        std::atomic<bool> started_{false};
        Event::EventCallbackEx cb_;
    //ioManage
        std::set<uint16_t> portManager_;
        Event::EventManager evManager_;  
        Route::Router routeManager_;
    private:
        void portManagerInit(uint16_t min_media_port = 10000, uint16_t max_media_port = 30000);
        void restManagerInit(uint16_t port = 8000);
        void onHttpConnect(Event::HttpEvPtr httpev);
        void onRequest(const Http::HttpRequest& req, Http::HttpResponse& rsp);

    //restManage
        uint16_t onCreate(const std::string& terno, const std::string& peerIp, const uint16_t& peerPort, Rest::Code& code);
        void onDelete(const std::string& terno, Rest::Code& code);
        void onRoute(const std::string& terno, const std::vector<std::string>& dst, Rest::Code& code){};
    };  
}