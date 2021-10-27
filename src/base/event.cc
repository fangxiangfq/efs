#include "event.h"
#include "eventsloop.h"
#include <assert.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <sys/time.h>

namespace Event
{
    const int Event::kNoneEvent = 0;
    const int Event::kReadEvent = EPOLLIN | EPOLLPRI;
    const int Event::kWriteEvent = EPOLLOUT;

    void Event::update() 
    {
        if(loop_)
            loop_->updateEvent(*this);
    }

    TaskEvent::TaskEvent(EventsLoop* loop)
    :Event(loop, EvType::task)
    {
        fd_.eventFd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if(fd_.fd < 0)
        {
            //todo log
            abort();
        }
    }

    void TaskEvent::read() const
    {

    }

    void TaskEvent::write() const
    {

    }

    TimerEvent::TimerEvent(long time, EventsLoop* loop, bool looptimer)
    :Event(loop, EvType::timer)
    {
        fd_.timerFd = ::timerfd_create(CLOCK_MONOTONIC, EFD_NONBLOCK | EFD_CLOEXEC);
        if(fd_.fd < 0)
        {
            //todo log
            abort();
        }
        set(time, loop);
    }

    void TimerEvent::set(long time, bool loop)
    {
        struct itimerspec new_value;
        new_value.it_value.tv_sec  = 0; 
        new_value.it_value.tv_nsec = time * 1000;
        
        new_value.it_interval.tv_sec  = 0; // one shot
        if(!loop)
            new_value.it_interval.tv_nsec = 0;
        else
            new_value.it_interval.tv_nsec = time * 1000;
        int ret = timerfd_settime(fd_.timerFd, 0, &new_value, NULL);
        if(ret < 0)
        {
            //todo log
            abort();
        }
    }

    void TimerEvent::read() const
    {

    }

    void TimerEvent::write() const
    {

    }

    UdpEvent::UdpEvent(const uint16_t& localPort, const uint16_t& peerPort, const std::string& peerIp, EventsLoop* loop)
    :Event(loop, EvType::udp), socket_()
    {
        socket_.bindAddress(Net::InetAddress(localPort));
        socket_.setPeerAddr(std::move(Net::InetAddress(peerIp, peerPort)));
        fd_.udpSockFd = socket_.fd();
        if(fd_.udpSockFd < 0)
        {
            //todo log
            abort();
        }
    }

    void UdpEvent::read() const
    {
         
    }

    void UdpEvent::write() const
    {

    }

    TcpListenEvent::TcpListenEvent(const uint16_t& localPort, bool isHttp, EventsLoop* loop)
    :Event(loop, EvType::tcplisten), socket_(Socket::SockType::tcplisten)
    {
        socket_.bindAddress(Net::InetAddress(localPort));
        socket_.listen();
        fd_.tcpListenFd = socket_.fd();
        if(fd_.tcpListenFd < 0)
        {
            //todo log
            abort();
        }
    }

    void TcpListenEvent::read() const
    {
         
    }

    TcpEvent::TcpEvent(const int& connfd, const Net::InetAddress& localAddr, const Net::InetAddress& peerAddr, EventsLoop* loop)
    :Event(loop, EvType::tcp), socket_(connfd, localAddr, peerAddr)
    {

    }

    void TcpEvent::read() const
    {
         
    }

    void TcpEvent::write() const
    {
         
    }

    HttpEvent::HttpEvent(const int& connfd, const Net::InetAddress& localAddr, const Net::InetAddress& peerAddr, EventsLoop* loop)
    :Event(loop, EvType::tcp), socket_(connfd, localAddr, peerAddr)
    {

    }

    void HttpEvent::read() const
    {
         
    }

    void HttpEvent::write() const
    {
         
    }
}