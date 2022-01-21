#include "router.h"

namespace Route
{
    void delFromDst(const Socket::SockInfo& src, const Socket::SockInfo& dst)
    {
        auto it = dstMap_.find(src);
        if(it != dstMap_.end())
        {
            it->second.erase(dst);
            if(0 == it->second.size())
            {
                dstMap_.erase(it);
            }   
        }
    }

    void delFromSrc(const Socket::SockInfo& src, const Socket::SockInfo& dst)
    {
        auto it = srcMap_.find(dst);
        if(it != srcMap_.end())
        {
            it->second.erase(src);
            if(0 == it->second.size())
            {
                srcMap_.erase(it);
            }   
        }
    }

    void Router::add(const Socket::SockInfo& src, const Socket::SockInfo& dst) 
    {
        auto it = dstMap_.find(src);
        if(it != dstMap_.end())
            it->second.emplace(dst);
        else
        {
            RouteSet s;
            s.emplace(dst);
            dstMap_.emplace(src, std::move(s));
        } 

        it = srcMap_.find(dst);
        if(it != srcMap_.end())
            it->second.emplace(src);
        else
        {
            RouteSet s;
            s.emplace(src);
            dstMap_.emplace(dst, std::move(s));
        } 
    }
    
    void Router::del(const Socket::SockInfo& src, const Socket::SockInfo& dst) 
    {
        delFromDst(src, dst);
        delFromSrc(src, dst);
    }
    
    void Router::del(const Socket::SockInfo& src) //may it unsafe
    {
        auto it = dstMap_.find(src);
        if(it != dstMap_.end())
        {
            for(auto it2 = it->second.begin(); it2 != it->second.end();)
            {
                delFromSrc(src, *it2);
                it2++;
            }
            dstMap_.erase(it);
        }

        it = srcMap_.find(src);
        if(it != srcMap_.end())
        {
            for(auto it2 = it->second.begin(); it2 != it->second.end();)
            {
                delFromDst(*it2, src);
                it2++;
            }
            srcMap_.erase(it);
        }
    }

    RouteManager::RouteManager(ThreadLocal::ThreadLocal& threadLocal)
    :tls_(threadLocal.allocateSlot())
    {
        tls_->set([](Event::EventsLoop*) -> ThreadLocal::ThreadLocalObjectSharedPtr {
            return std::make_shared<Router>();
        });
    }

    Router& RouteManager::router()
    {
        return tls_->getTyped<Router>();
    }

    void RouteManager::add(const Socket::SockInfo& src, const Socket::SockInfo& dst)
    {
        tls_->runOnAllThreads([this, src, dst]{
            Router& r = tls_->getTyped<Router>();
            r.add(src, dst);
        });
    }

    void RouteManager::del(const Socket::SockInfo& src, const Socket::SockInfo& dst)
    {
        tls_->runOnAllThreads([this, src, dst]{
            Router& r = tls_->getTyped<Router>();
            r.del(src, dst);
        });
    }

    void RouteManager::del(const Socket::SockInfo& src)
    {
        tls_->runOnAllThreads([this, src]{
            Router& r = tls_->getTyped<Router>();
            r.del(src);
        });
    }
}
