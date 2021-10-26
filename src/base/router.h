#pragma once

#include <map>
#include "threadlocal.h"
#include "sockets.h"

namespace Route
{
    struct Router : public ThreadLocal::ThreadLocalObject
    {
        std::multimap<int, Socket::SockInfo> dstMap_;
        std::multimap<int, int> srcMap_;
    };

    class RouteManger
    {
    private:
        ThreadLocal::SlotPtr tls_;
    public:
        RouteManger(ThreadLocal::ThreadLocal& threadLocal);
        ~RouteManger()=default;
        Router& router();
        void add(int& src, Socket::SockInfo& dst);
        void del(int& src, Socket::SockInfo& dst);
        void del(int& src);
    };
    using RouteMap = std::multimap<int, Socket::SockInfo>;
}

