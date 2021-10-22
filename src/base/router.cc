#include "router.h"

namespace Route
{
    RouteManger::RouteManger(ThreadLocal::ThreadLocal& threadLocal, Event::EventsLoop* loop)
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

    void RouteManger::add(int& src, Socket::Sockinfo& dst)
    {
        tls_->runOnAllThreads([src, dst]{
            tls_->getTyped<Router>().dstMap_.emplace(src, dst);
            tls_->getTyped<Router>().srcMap_.emplace(dst.sockfd_, src);
        });
    }

    void RouteManger::del(int& src, Socket::Sockinfo& dst)
    {
        tls_->runOnAllThreads([src, dst]{
            Router& r = tls_->getTyped<Router>();
            for(auto it = r.dstMap_.lower_bound(src); it != r.dstMap_.upper_bound(src);)
            {
               if(dst == it->second)
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

    void RouteManger::del(int& src)
    {
        tls_->runOnAllThreads([src, dst]{
            for(auto it = r.dstMap_.lower_bound(src); it != r.dstMap_.upper_bound(src);)
            {
                int dst = it->second();
                for(auto it2 = r.srcMap_.lower_bound(dst); it2 != r.srcMap_.upper_bound(dst);)
                {
                    if(src == it->second)
                        r.srcMap_.erase(it2++);
                    else
                        it2++;
                }

                r.dstMap_.erase(it++);
            }
        });
    }
}
