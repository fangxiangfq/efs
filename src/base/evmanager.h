#pragma once

#include <memory>
#include <string>
#include <set>
#include <map>
#include "event.h"

namespace Event
{
    using TaskEvPtr = std::shared_ptr<TaskEvent>;
    using TcpListenEvPtr = std::unique_ptr<TcpListenEvent>;
    class EventManager final
    {
    public:
        std::set<HttpEvPtr> httpConManager_;
        std::map<std::string, UdpEvPtr> termManager_;
        TcpListenEvPtr restManager_{};

    public:
        EventManager()=default;
        ~EventManager()=default;
        static TaskEvPtr createTaskEvPtr()
        {
            return std::make_shared<TaskEvent>();
        }

        static TaskEvent&& createTaskEv()
        {
            return std::move(TaskEvent());
        }

        static HttpEvent&& createHttpEv(const int& connfd, const Net::InetAddress& localAddr, const Net::InetAddress& peerAddr)
        {
            return std::move(HttpEvent(connfd, localAddr, peerAddr));
        }

        static HttpEvent&& createHttpEv(const int& connfd, const Net::InetAddress& localAddr, const Net::InetAddress& peerAddr, EventsLoop* loop)
        {
            return std::move(HttpEvent(connfd, localAddr, peerAddr, loop));
        }

        static HttpEvPtr createHttpEvPtr(const int& connfd, const Net::InetAddress& localAddr, const Net::InetAddress& peerAddr)
        {
            return std::make_shared<HttpEvent>(connfd, localAddr, peerAddr);
        }

        static HttpEvPtr createHttpEvPtr(const int& connfd, const Net::InetAddress& localAddr, const Net::InetAddress& peerAddr, EventsLoop* loop)
        {
            return std::make_shared<HttpEvent>(connfd, localAddr, peerAddr, loop);
        }

        static UdpEvPtr createUdpEvPtr(const uint16_t& localPort, const uint16_t& peerPort, const std::string& peerIp, EventsLoop* loop)
        {
            return std::make_shared<UdpEvent>(localPort, peerPort, peerIp, loop);
        }

        static TimerEvPtr createTimerEvPtr(const EventCallback& cb, long time, bool looptimer, EventsLoop* loop)
        {
            return std::make_shared<TimerEvent>(cb, time, looptimer, loop);
        }

        static TimerEvPtr createTimerEvPtr(const EventCallback& cb, long time, bool looptimer)
        {
            return std::make_shared<TimerEvent>(cb, time, looptimer);
        }

        static TimerEvPtr createTimerEvPtr(const EventCallback& cb, long time)
        {
            return std::make_shared<TimerEvent>(cb, time);
        }

        static SockPairPtrPair createSockPairPtrPair(EventsLoop* loop)
        {
            Socket::SocketPair sockpair; 
            return std::make_pair(std::make_shared<SockPairEvent>(sockpair.first(), true, loop), std::make_shared<SockPairEvent>(sockpair.second(), false));
        }
    };
}

using EvManager = Event::EventManager;