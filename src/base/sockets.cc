#include "sockets.h"
#include <stdio.h>
#include <string.h>
#include <sys/uio.h>  // readv
#include <unistd.h>
using namespace Net;
namespace Socket
{
    SocketPair::SocketPair()
    :first_(-1), second_(-1) 
    {
        int socket_pair[2] = {-1, -1};
        if(0 > ::socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0, socket_pair))
        {
            //todo log
        }
        
        first_ = socket_pair[0];
        second_ = socket_pair[1];
    }
    
    SocketPair::~SocketPair() 
    {
        ::close(first_);
        ::close(second_);
    }
    
    Socket::Socket(SockType type) 
    : sockfd_(-1), type_(type)
    {
        if(SockType::udp == type)
            sockfd_ = ::socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_UDP);
        else if(SockType::tcp == type)
            sockfd_ = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);

        if(0 > sockfd_)
        {
            //todo log
        }
    }
    
    Socket::Socket(int fd, SockType type) 
    :sockfd_(fd), type_(type)
    {
        
    }

    Socket::~Socket()
    {
        ::close(sockfd_);
    }
    
    
    void Socket::bindAddress(const InetAddress& localaddr) 
    {
        int ret = ::bind(sockfd_, localaddr.getSockAddr(), static_cast<socklen_t>(sizeof(struct sockaddr_in)));
        if(ret < 0)
        {
            //todo log
        }
    }
    
    void Socket::listen() 
    {
        int ret = ::listen(sockfd_, SOMAXCONN);
        if(ret < 0)
        {
            //todo log
        }
    }
    
    int Socket::accept(InetAddress* peeraddr) 
    {
        struct sockaddr_in addr;
        socklen_t addrlen = static_cast<socklen_t>(sizeof addr);
        memset(&addr, 0, sizeof addr);
        int connfd = ::accept(sockfd_, sockaddr_cast(&addr), &addrlen);
        if(connfd >= 0)
        {
            peeraddr->setSockAddr(addr);
        }

        return connfd;
    }
    
    void Socket::setReuseAddr(bool on) 
    {
        int optval = on ? 1 : 0;
        ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
            &optval, static_cast<socklen_t>(sizeof optval));
    }
    
    void Socket::setKeepAlive(bool on) 
    {
        int optval = on ? 1 : 0;
        ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
               &optval, static_cast<socklen_t>(sizeof optval));
    }

    // struct sockaddr_in6 getLocalAddr(int sockfd)
    // {
    //     struct sockaddr_in6 localaddr;
    //     memset(&localaddr, sizeof localaddr);
    //     socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
    //     if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0)
    //     {
            
    //     }
    //     return localaddr;
    // }

    // struct sockaddr_in6 getPeerAddr(int sockfd)
    // {
    //     struct sockaddr_in6 peeraddr;
    //     memset(&peeraddr, sizeof peeraddr);
    //     socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
    //     if (::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0)
    //     {
            
    //     }
    //     return peeraddr;
    // }
}