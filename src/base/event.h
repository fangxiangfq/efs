#pragma once

#include <functional>
#include <memory>
#include <assert.h>
#include <unistd.h>
#include "sockets.h"
#include "http.h"
#include "buffer.h"

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
        void setLoop(EventsLoop* loop){ if(NULL == loop_){loop_= loop;} }

        int fd() const { return fd_.fd; }
        int events() const { return events_; }
        EvType getType() const { return type_; }
        void enableRead() { events_ |= kReadEvent; update(); }
        void disableRead() { events_ &= ~kReadEvent; update(); }
        void enablewrite(Buffer::Buffer& buf) { events_ |= kWriteEvent; update(); }
        void disablewrite(Buffer::Buffer& buf) { events_ &= ~kWriteEvent; update(); }
        void disableAll() { events_ = kNoneEvent; update(); }
        bool isWriting() const { return events_ & kWriteEvent; }
        bool isReading() const { return events_ & kReadEvent; }
        bool isNoneEvent() const { return events_ == kNoneEvent; }
        void setReadCb(const EventCallback& readCb) { readCb_ = readCb; }
        void remove();
        EventsLoop* ownerLoop() const { return loop_; }

        virtual void read() = 0;
        virtual void write(Buffer::Buffer& buf) = 0;
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
        static const size_t kMaxTcpMsgLen;
    };

    class TaskEvent : public Event
    {
    public:
        TaskEvent(EventsLoop* loop = NULL);
        ~TaskEvent() { ::close(fd_.fd); }
        void read() override;
        void write(Buffer::Buffer& buf) override;
    };

    class TimerEvent : public Event
    {
    public:
        TimerEvent(long time, EventsLoop* loop = NULL, bool looptimer = false);//us
        ~TimerEvent() { ::close(fd_.fd); }
        void set(long time, bool loop);
        void read() override;
        void write(Buffer::Buffer& buf) override;
    };
    class UdpEvent : public Event
    {
    public:
        UdpEvent(const uint16_t& localPort, const uint16_t& peerPort, const std::string& peerIp, EventsLoop* loop = NULL);
        ~UdpEvent()=default;
        void read() override;
        void write(Buffer::Buffer& buf) override;
    private:
        Socket::Socket socket_;
    };
    using UdpEvPtr = std::shared_ptr<UdpEvent>;

    class TcpEvent : public Event
    {
    public:
        TcpEvent(const int& connfd, const Net::InetAddress& localAddr, const Net::InetAddress& peerAddr, EventsLoop* loop = NULL);
        void read() override;
        void write(Buffer::Buffer& buf) override;
    private:
        Socket::Socket socket_;
    };
    
    class HttpEvent;
    using HttpRequestCb = std::function<void(const Http::HttpRequest& req, Http::HttpResponse& rsp)>;
    using HttpEvPtr = std::shared_ptr<HttpEvent>;
    using ConnectCb = std::function<void(HttpEvPtr)>;
    using HttpCloseCb = std::function<void(HttpEvPtr)>;

    class HttpEvent : public Event , public std::enable_shared_from_this<HttpEvent> 
    {
    public:
        HttpEvent(const int& connfd, const Net::InetAddress& localAddr, const Net::InetAddress& peerAddr, EventsLoop* loop = NULL);
        ~HttpEvent()=default;
        void read() override;
        void write(Buffer::Buffer& buf) override;
        void write(std::string str);
        void onClose();
        void setReqCb(const HttpRequestCb& cb) { reqcb_ = cb; }
        void setCloseCb(const HttpCloseCb& cb) { closecb_ = cb; }
        Http::HttpContext& context() {return httpctx_; } 
    private:
        void onRequest(const Http::HttpRequest& req);
        void handleClose();
        void shutdown();

        bool isNeedShutdown_;
        HttpRequestCb reqcb_;
        HttpCloseCb closecb_;
        Socket::Socket socket_;
        Http::HttpContext httpctx_;
    };

    class TcpListenEvent : public Event
    {
    public:
        TcpListenEvent(const ConnectCb& cb, const uint16_t& localPort, bool isHttp = true, EventsLoop* loop = NULL);
        ~TcpListenEvent();
        void read() override;
        void write(Buffer::Buffer& buf) override {}//do nothing
        bool isHttpListening() const { return isHttpListening_; }
    private:
        ConnectCb conncb_;
        bool isHttpListening_;
        Socket::Socket socket_;
    };
} // namespace Event
  
 