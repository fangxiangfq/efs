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
        RouteMap srcMap_;
        void delFromDst(const Socket::SockInfo& src, const Socket::SockInfo& dst);
        void delFromSrc(const Socket::SockInfo& src, const Socket::SockInfo& dst);
    public:
        void add(const Socket::SockInfo& src, const Socket::SockInfo& dst);
        void del(const Socket::SockInfo& src, const Socket::SockInfo& dst);
        void del(const Socket::SockInfo& src);

        const RouteMap& dstmap() { return dstMap_; };
        const RouteMap& srcmap() { return srcMap_; };
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

