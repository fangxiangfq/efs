#pragma once
#include <sys/socket.h>
#include <vector>
#include <memory>

namespace Net{class InetAddress;}
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

    // struct sockaddr_in6 getLocalAddr(int sockfd);
    // struct sockaddr_in6 getPeerAddr(int sockfd);

}// namespace socket