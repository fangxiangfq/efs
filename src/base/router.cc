#include "router.h"

namespace Route
{
    RouteManger::RouteManger(ThreadLocal::ThreadLocal& threadLocal)
    :tls_(threadLocal.allocateSlot())
    {
        tls_->set([](Event::EventsLoop*) -> ThreadLocal::ThreadLocalObjectSharedPtr {
            return std::make_shared<Router>();
        });
    }

    Router& RouteManger::router()
    {
        return tls_->getTyped<Router>();
    }

    void RouteManger::add(int& src, Socket::SockInfo& dst)
    {
        tls_->runOnAllThreads([this, src, dst]{
            tls_->getTyped<Router>().dstMap_.emplace(src, dst);
            tls_->getTyped<Router>().srcMap_.emplace(dst.sockfd_, src);
        });
    }

    void RouteManger::del(int& src, Socket::SockInfo& dst)
    {
        tls_->runOnAllThreads([this, src, dst]{
            Router& r = tls_->getTyped<Router>();
            for(auto it = r.dstMap_.lower_bound(src); it != r.dstMap_.upper_bound(src);)
            {
               if(it->second == dst)
                    r.dstMap_.erase(it++);
                else
                    it++;
            }
            for(auto it = r.srcMap_.lower_bound(dst.sockfd_); it != r.srcMap_.upper_bound(dst.sockfd_);)
            {
                if(src == it->second)
                    r.srcMap_.erase(it++);
                else
                    it++;
            }
        });
    }

    void RouteManger::del(int& fd)
    {
        tls_->runOnAllThreads([this, fd]{
            Router& r = tls_->getTyped<Router>();
            for(auto it = r.dstMap_.lower_bound(fd); it != r.dstMap_.upper_bound(fd);)
            {
                int dst = it->second.sockfd_;
                for(auto it2 = r.srcMap_.lower_bound(dst); it2 != r.srcMap_.upper_bound(dst);)
                {
                    if(fd == it2->second)
                        r.srcMap_.erase(it2++);
                    else
                        it2++;
                }
                r.dstMap_.erase(it++);
            }
            for(auto it = r.srcMap_.lower_bound(fd); it != r.srcMap_.upper_bound(fd);)
            {
                int src = it->second;
                for(auto it2 = r.dstMap_.lower_bound(src); it2 != r.dstMap_.upper_bound(src);)
                {
                    if(fd == it2->second.sockfd_)
                        r.dstMap_.erase(it2++);
                    else
                        it2++;
                }
                r.srcMap_.erase(it++);
            }
        });
    }
}
