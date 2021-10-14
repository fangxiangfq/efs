#pragma once
#include <sys/socket.h>
#include <vector>

namespace Net{class InetAddress;}
namespace Socket
{
    using SocketPairArr = std::vector<Socket::SocketPair>; 
    class SocketPair
    {
    public:
        explicit SocketPair();
        ~SocketPair();
        SocketPair(const SocketPair&)=delete;
        SocketPair operator=(const SocketPair&)=delete;
        int get_first(){return first_;}
        int get_second(){return second_;}
    private:
        int first_;
        int second_;     
    };

    enum class TransType
    { 
        udp,
        tcp,
        //local
    };
//only support noblocking
    class Socket
    {

    public:
        explicit Socket(TransType type = TransType::udp);
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
        TransType type_;
    };
}// namespace socket