#pragma once
#include <sys/socket.h>
#include <vector>
#include <memory>
#include "inetaddress.h"

namespace Socket
{
    class SocketPair
    {
    public:
        explicit SocketPair();
        ~SocketPair();
        SocketPair(const SocketPair&)=delete;
        SocketPair operator=(const SocketPair&)=delete;
        int first(){return first_;}
        int second(){return second_;}
    private:
        int first_;
        int second_;     
    };

    using SocketPtr = std::unique_ptr<SocketPair>;
    using SocketPairArr = std::vector<SocketPtr>; 

    enum class SockType
    { 
        udp,
        tcp,
        tcplisten,
    };

//only support noblocking
    class SockInfo
    {
    public:
        SockInfo(const int &fd, const uint16_t& local_port, const uint16_t& peerPort, const std::string& peerIp)
        :peerAddr_(peerIp, peerPort), localAddr_(local_port), sockfd_(fd) {}
        SockInfo(const int &fd, const Net::InetAddress& localAddr, const Net::InetAddress& peerAddr)
        :peerAddr_(peerAddr), localAddr_(localAddr), sockfd_(fd) {}
        SockInfo(const int &fd)
        :peerAddr_(), localAddr_(), sockfd_(fd) {}
        //op == reload
        bool operator==(const SockInfo& rhs) const
        {
            return (sockfd_ == rhs.sockfd_) &&
                   (localAddr_.port() == rhs.localAddr_.port()) &&
                   (localAddr_.ip() == rhs.localAddr_.ip());
                   (peerAddr_.port() == rhs.peerAddr_.port()) &&
                   (peerAddr_.ip() == rhs.peerAddr_.ip());
        }

        bool operator<(const SockInfo& rhs) const
        {
            return (sockfd_ < rhs.sockfd_);
        }

        Net::InetAddress peerAddr_;
        Net::InetAddress localAddr_;
        int sockfd_;
    };

    using SocketInfoPtr = std::shared_ptr<SocketPair>;
    class Socket
    {
    public:
        explicit Socket(SockType type = SockType::udp);
        explicit Socket(const int& connfd, const Net::InetAddress& localAddr, const Net::InetAddress& peerAddr, SockType type = SockType::tcp);
        ~Socket();
        Socket(const Socket&)=delete;
        Socket operator=(const Socket&)=delete;
        int  fd() const { return sock_.sockfd_; }
        void bindAddress(const Net::InetAddress& localaddr);
        void listen();
        int  accept(Net::InetAddress* peeraddr);
        void setReuseAddr(bool on);
        void setKeepAlive(bool on);
        void setPeerAddr(Net::InetAddress&& peerAddr){ sock_.peerAddr_ = peerAddr;}

    private:
        SockInfo sock_;
        SockType type_;
    }; 

    struct sockaddr_in getLocalAddr(int sockfd);
    struct sockaddr_in getPeerAddr(int sockfd);

}// namespace socket