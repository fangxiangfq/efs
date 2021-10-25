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
        tcplink,
    };
//only support noblocking
    class Socket
    {
    public:
        explicit Socket(SockType type = SockType::udp);
        explicit Socket(int fd, SockType type = SockType::udp);
        ~Socket();
        Socket(const Socket&)=delete;
        Socket operator=(const Socket&)=delete;
        int fd() const { return sockfd_; }
        void bindAddress(const Net::InetAddress& localaddr);
        void listen();
        int accept(Net::InetAddress* peeraddr);
        void setReuseAddr(bool on);
        void setKeepAlive(bool on);

    private:
        int sockfd_;
        SockType type_;
    }; 

    class Sockinfo
    {
    public:
        Sockinfo(const uint16_t& local_port, const int &fd, const uint16_t& peer_port, const std::string& ip)
        :PerrAddr_(ip, peer_port), local_port_(local_port), sockfd_(fd) {}
        //op == reload
        bool operator==(const Sockinfo& rhs)
        {
            return (sockfd_ == rhs.sockfd_) &&
                   (local_port_ == rhs.local_port_) &&
                   (PerrAddr_.port() == rhs.PerrAddr_.port()) &&
                   (PerrAddr_.ip() == rhs.PerrAddr_.ip());
        }

        Net::InetAddress PerrAddr_;
        uint16_t    local_port_;
        int         sockfd_;
    };

    struct sockaddr_in getLocalAddr(int sockfd);
    struct sockaddr_in getPeerAddr(int sockfd);

}// namespace socket