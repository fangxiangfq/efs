#pragma once

#include <functional>
#include <memory>
#include <assert.h>
#include "sockets.h"
#include "http.h"

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

        int fd() const { return fd_.fd; }
        int events() const { return events_; }
        EvType type() const { return type_; }
        void enableRead() { events_ |= kReadEvent; update(); }
        void disableRead() { events_ &= ~kReadEvent; update(); }
        void enableWrite() { events_ |= kWriteEvent; update(); }
        void disableWrite() { events_ &= ~kWriteEvent; update(); }
        void disableAll() { events_ = kNoneEvent; update(); }
        bool isWriting() const { return events_ & kWriteEvent; }
        bool isReading() const { return events_ & kReadEvent; }
        bool isNoneEvent() const { return events_ == kNoneEvent; }
        void setReadCb(const EventCallback& readCb) { readCb_ = readCb; }
        EventsLoop* ownerLoop() const { return loop_; }

        virtual void read() = 0;
        virtual void write() = 0;
    protected:
        void update();
        EventsLoop* loop_;
        BaseFd fd_;
        int events_{0};
        EvType type_; 
        EventCallback readCb_;
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
        TimerEvent(long time, EventsLoop* loop = NULL, bool looptimer = false);//us
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
    using UdpEvPtr = std::shared_ptr<UdpEvent>;

    class TcpEvent : public Event
    {
    public:
        TcpEvent(const int& connfd, const Net::InetAddress& localAddr, const Net::InetAddress& peerAddr, EventsLoop* loop = NULL);
        void read() override;
        void write() override;
    private:
        Socket::Socket socket_;
    };

    class HttpEvent : public Event
    {
    public:
        HttpEvent(const int& connfd, const Net::InetAddress& localAddr, const Net::InetAddress& peerAddr, EventsLoop* loop = NULL);
        void read() override;
        void write() override;
        Http::HttpContext& context() {return httpctx_; } 
    private:
        Socket::Socket socket_;
        Http::HttpContext httpctx_;
    };

    using HttpEvPtr = std::shared_ptr<HttpEvent>;
    using ConnectCb = std::function<void(HttpEvPtr)>;
    class TcpListenEvent : public Event
    {
    public:
        TcpListenEvent(const ConnectCb& cb, const uint16_t& localPort, bool isHttp = true, EventsLoop* loop = NULL);
        void read() override;
        void write() override {}//do nothing
        bool isHttpListening() const { return isHttpListening_; }
    private:
        ConnectCb conncb_;
        bool isHttpListening_;
        Socket::Socket socket_;
    };
} // namespace Event
  
 