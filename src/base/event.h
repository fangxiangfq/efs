#pragma once

#include <functional>
#include <memory>
#include <assert.h>
#include "sockets.h"

namespace Event
{
    class EventsLoop;
    class Event;
    using EventCallback = std::function<void()>;
    using EventCallbackEx = std::function<void(Event&)>;

    union BaseFd
    {
        int fd;
        //
        int eventFd;
        int spFirst;
        int spSecond;
        int timerFd;
        int signalFd;
        int udpSockFd;
        int tcpListenFd;
        int tcpSockFd;
        int httpSockFd;
    };

    enum class EvType
    {
        task,
        sockpair,
        timer,
        signal,
        udp,
        tcplisten,
        tcp,
        http,
        file
    };

    class Event //:public std::enable_shared_from_this<Event> 
    {
    public:
        Event(EventsLoop* loop, EvType type) : loop_(loop), fd_(), type_(type) {}
        virtual ~Event()=default;
        void setLoop(EventsLoop* loop){ if(!loop_){loop_= loop;} }

        BaseFd fd() const { return fd_; }
        int events() const { return events_; }
        EvType evtype() const { return type_; }
        void enableRead() { events_ |= kReadEvent; update(); }
        void disableRead() { events_ &= ~kReadEvent; update(); }
        void enableWrite() { events_ |= kWriteEvent; update(); }
        void disableWrite() { events_ &= ~kWriteEvent; update(); }
        void disableAll() { events_ = kNoneEvent; update(); }
        bool isWriting() const { return events_ & kWriteEvent; }
        bool isReading() const { return events_ & kReadEvent; }
        bool isNoneEvent() const { return events_ == kNoneEvent; }
        EventsLoop* ownerLoop() const { return loop_; }

        virtual void read() = 0;
        virtual void write() = 0;
    protected:
        void update();
        EventsLoop* loop_;
        BaseFd fd_;
        int events_{0};
        EvType type_; 

        static const int kNoneEvent;
        static const int kReadEvent;
        static const int kWriteEvent;
    };

    class TaskEvent : public Event
    {
    public:
        TaskEvent(EventsLoop* loop = NULL);
        void read() override;
        void write() override;
    };

    class TimerEvent : public Event
    {
    public:
        TimerEvent(long time, EventsLoop* loop = NULL, bool looptimer = false);//ms
        void set(long time, bool loop);
        void read() override;
        void write() override;
    };

    class UdpEvent : public Event
    {
    public:
        UdpEvent(const uint16_t& localPort, const uint16_t& peerPort, const std::string& peerIp, EventsLoop* loop = NULL);
        void read() override;
        void write() override;
    private:
        Socket::Socket socket_;
    };

    class TcpListenEvent : public Event
    {
    public:
        TcpListenEvent();
        void read() override;
        void write() override;
    };

    class TcpEvent : public Event
    {
    public:
        TcpEvent();
        void read() override;
        void write() override;
    };

    class HttpEvent : public Event
    {
    public:
        HttpEvent();
        void read() override;
        void write() override;
    };
} // namespace Event
  
 