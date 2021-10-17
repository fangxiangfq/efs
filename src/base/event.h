#pragma once

#include <functional>
#include <memory>
#include <assert.h>
#include "sockets.h"

namespace Event
{
    class EventsLoop;

    using EventCallback = std::function<void()>;

    enum class EvType
    {
        udp,
        tcplink,
        tcp,
        local
    };

    class Event //:public std::enable_shared_from_this<Event> 
    {
    public:
        Event(EventsLoop* loop, int fd, EvType type = EvType::local, uint16_t port = 0);
        ~Event()=default;
        int fd() const { return fd_; }
        int port() const { return port_; }
        EvType type() const { return type_; }
        void read() const { readCallback_(); }
        int events() const { return events_; }
        void enableRead() { events_ |= kReadEvent; update(); }
        void disableRead() { events_ &= ~kReadEvent; update(); }
        void enableWrite() { events_ |= kWriteEvent; update(); }
        void disableWrite() { events_ &= ~kWriteEvent; update(); }
        void disableAll() { events_ = kNoneEvent; update(); }
        bool isWriting() const { return events_ & kWriteEvent; }
        bool isReading() const { return events_ & kReadEvent; }
        void setRead(const EventCallback& cb) {readCallback_ = cb; }
        void setWrite(const EventCallback& cb) {writeCallback_ = cb; }
        bool isNoneEvent() const { return events_ == kNoneEvent; }
        EventsLoop* ownerLoop() const { return loop_; }
        void remove();
   
    private:
        void update();
        EventsLoop* loop_;
        const int fd_;
        int events_;
        EventCallback readCallback_;
        EventCallback writeCallback_;
        EvType type_; 
        uint16_t port_;
        static const int kNoneEvent;
        static const int kReadEvent;
        static const int kWriteEvent;

    };

} // namespace Event
  
 