#include "event.h"
#include <assert.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <sys/time.h>
#include "eventsloop.h"
#include "evmanager.h"
#include "sockets.h"
#include "logger.h"

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

    void Event::remove()
    {
        if(loop_)
            loop_->removeEvent(*this);
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

    void TaskEvent::read()
    {

    }

    void TaskEvent::write(Buffer::Buffer& buf)
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

    void TimerEvent::read()
    {

    }

    void TimerEvent::write(Buffer::Buffer& buf)
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

    void UdpEvent::read()
    {
         
    }

    void UdpEvent::write(Buffer::Buffer& buf) 
    {

    }

    TcpListenEvent::TcpListenEvent(const ConnectCb& cb, const uint16_t& localPort, bool isHttp, EventsLoop* loop)
    :Event(loop, EvType::tcplisten), conncb_(cb), isHttpListening_(isHttp), socket_(Socket::SockType::tcplisten)
    {
        socket_.setReuseAddr(true);
        socket_.setReusePort(false);
        socket_.bindAddress(Net::InetAddress(localPort));
        socket_.listen();
        fd_.tcpListenFd = socket_.fd();
        if(fd_.tcpListenFd < 0)
        {
            //todo log
            abort();
        } 
        STD_ERROR("cons");
    }

    TcpListenEvent::~TcpListenEvent()
    {
        STD_ERROR("cons");
    }

    void TcpListenEvent::read()
    {
        struct sockaddr_in addr;
        socklen_t addrlen = static_cast<socklen_t>(sizeof addr);
        int connfd = ::accept(fd_.tcpListenFd, Net::sockaddr_cast(&addr), &addrlen);
        Net::InetAddress localAddr(Socket::getLocalAddr(connfd));
        Net::InetAddress peerAddr(addr);
        HttpEvPtr httpev = EventManager::createHttpEvPtr(connfd, localAddr, peerAddr);
        if(conncb_)
            conncb_(httpev);
    }

    TcpEvent::TcpEvent(const int& connfd, const Net::InetAddress& localAddr, const Net::InetAddress& peerAddr, EventsLoop* loop)
    :Event(loop, EvType::http), socket_(connfd, localAddr, peerAddr)
    {

    }

    void TcpEvent::read() 
    {
         
    }

    void TcpEvent::write(Buffer::Buffer& buf)
    {
         
    }

    HttpEvent::HttpEvent(const int& connfd, const Net::InetAddress& localAddr, const Net::InetAddress& peerAddr, EventsLoop* loop)
    :Event(loop, EvType::http), isNeedShutdown_(false), socket_(connfd, localAddr, peerAddr)
    {
        fd_.httpSockFd = connfd;
        if(fd_.httpSockFd < 0)
        {
            //todo log
            abort();
        }
    }

    void HttpEvent::read()
    {
        Buffer::Buffer buf(128);
        int savedErrno = 0;
        ssize_t n = buf.recv(fd_.httpSockFd, &savedErrno, 4);
        if(n == 0)
        {
            handleClose();
            return;
        }
        else if(n < 0)
        {
            if((savedErrno == EAGAIN || savedErrno == EWOULDBLOCK))
            {
                return;
            } 
            handleClose();
        }

        uint32_t len = 0;
        buf.write(&len);
        n = buf.recv(fd_.httpSockFd, &savedErrno, static_cast<size_t>(len));
        if(n == 0)
        {
            handleClose();
            return;
        }
        else if(n < 0)
        {
            if((savedErrno == EAGAIN || savedErrno == EWOULDBLOCK))
            {
                return;
            }
            shutdown();
        }

        if(!httpctx_.parseRequest(&buf))
        {
            write("HTTP/1.1 400 Bad Request\r\n\r\n");
            shutdown();
            return;
        }

        if(httpctx_.gotAll())
        {
            onRequest(httpctx_.request());
            httpctx_.reset();
        }
    }

    void HttpEvent::write(Buffer::Buffer& buf) 
    {
        int savedErrno = 0;
        ssize_t n = buf.send(fd_.httpSockFd, &savedErrno);
        if(n == 0)
        {
            handleClose();
            return;
        }
        else if(n < 0)
        {
            if((savedErrno == EAGAIN || savedErrno == EWOULDBLOCK))
            {
                return;
            }
            handleClose(); 
        }
    }

    void HttpEvent::write(std::string str)
    {
        int savedErrno = 0;
        ssize_t n = ::send(fd_.httpSockFd, str.c_str(), str.length(), 0);

        if(n == 0)
        {
            handleClose();
            return;
        }
        else if(n < 0)
        {
            if((savedErrno == EAGAIN || savedErrno == EWOULDBLOCK))
            {
                return;
            }
            handleClose(); 
        }
    }

    void HttpEvent::onRequest(const Http::HttpRequest& req) 
    {
        const std::string& connection = req.getHeader("Connection");
        bool close = connection == "close" ||
            (req.getVersion() == Http::HttpRequest::Version::kHttp10 && connection != "Keep-Alive");
        Http::HttpResponse rsp(close);

        if(reqcb_)
        {
            reqcb_(req, rsp);
        }

        Buffer::Buffer buf(128);
        rsp.appendToBuffer(&buf);

        write(buf);

        if (rsp.closeConnection())
        {
            shutdown();
        }
    }

    void HttpEvent::shutdown() 
    {
        socket_.shutdownWrite();
    }

    void HttpEvent::onClose()
    {
        disableAll();
        remove();
    }

    void HttpEvent::handleClose()
    {
        HttpEvPtr guardThis(shared_from_this());
        disableAll();
        if(closecb_)
            closecb_(guardThis);
    }
}