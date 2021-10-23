#pragma once

#include "threadlocal.h"
#include <map>
#include "sockets.h"

namespace Route
{
    class RouteManger
    {
    private:
        struct Router : public ThreadLocal::ThreadLocalObject
        {
            std::multimap<int, Socket::Sockinfo> dstMap_;
            std::multimap<int, int> srcMap_;
        }
        
        ThreadLocal::SlotPtr tls_;
        RouterPtr newRouter_{};
    public:
        RouteManger(ThreadLocal::ThreadLocal& threadLocal);
        ~RouteManger()=default;
        using RouterPtr = std::shared_ptr<Router>; 
        Router& router();
        void add(int& src, Socket::Sockinfo& dst);
        void del(int& src, Socket::Sockinfo& dst);
        void del(int& src);
    };
}

