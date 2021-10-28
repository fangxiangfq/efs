#pragma once

#include <map>
#include <set>
#include "threadlocal.h"
#include "sockets.h"

namespace Route
{
    using RouteSet = std::set<Socket::SockInfo>;
    using RouteMap = std::map<Socket::SockInfo, RouteSet>;
    class Router : public ThreadLocal::ThreadLocalObject
    {
    private:
        RouteMap dstMap_;
    public:
        void add(const Socket::SockInfo& src, const Socket::SockInfo& dst);
        void del(const Socket::SockInfo& src, const Socket::SockInfo& dst);
        void del(const Socket::SockInfo& src);
    };

    class RouteManager
    {
    private:
        ThreadLocal::SlotPtr tls_;
    public:
        RouteManager(ThreadLocal::ThreadLocal& threadLocal);
        ~RouteManager()=default;
        Router& router();
        void add(const Socket::SockInfo& src, const Socket::SockInfo& dst);
        void del(const Socket::SockInfo& src, const Socket::SockInfo& dst);
        void del(const Socket::SockInfo& src);
    };
}

