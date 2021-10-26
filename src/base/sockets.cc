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
    : sock_(-1), type_(type)
    {
        if(SockType::udp == type)
            sock_.sockfd_ = ::socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_UDP);
        else if(SockType::tcp == type)
            sock_.sockfd_ = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);

        if(0 > sock_.sockfd_)
        {
            //todo log
        }
    }
    
    Socket::Socket(int fd, SockType type) 
    :sock_(fd), type_(type)
    {
        
    }

    Socket::~Socket()
    {
        if(sock_.sockfd_ > 0)
            ::close(sock_.sockfd_);
    }
    
    
    void Socket::bindAddress(const InetAddress& localaddr) 
    {
        int ret = ::bind(sock_.sockfd_, localaddr.getSockAddr(), static_cast<socklen_t>(sizeof(struct sockaddr_in)));
        if(ret < 0)
        {
            //todo log
        }
    }
    
    void Socket::listen() 
    {
        int ret = ::listen(sock_.sockfd_, SOMAXCONN);
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
        int connfd = ::accept(sock_.sockfd_, sockaddr_cast(&addr), &addrlen);
        if(connfd >= 0)
        {
            peeraddr->setSockAddr(addr);
        }

        return connfd;
    }
    
    void Socket::setReuseAddr(bool on) 
    {
        int optval = on ? 1 : 0;
        ::setsockopt(sock_.sockfd_, SOL_SOCKET, SO_REUSEADDR,
            &optval, static_cast<socklen_t>(sizeof optval));
    }
    
    void Socket::setKeepAlive(bool on) 
    {
        int optval = on ? 1 : 0;
        ::setsockopt(sock_.sockfd_, SOL_SOCKET, SO_KEEPALIVE,
               &optval, static_cast<socklen_t>(sizeof optval));
    }

    struct sockaddr_in getLocalAddr(int sockfd)
    {
        struct sockaddr_in localaddr;
        memset(&localaddr, 0, sizeof localaddr);
        socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
        if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0)
        {
            
        }
        return localaddr;
    }

    struct sockaddr_in getPeerAddr(int sockfd)
    {
        struct sockaddr_in peeraddr;
        memset(&peeraddr, 0, sizeof peeraddr);
        socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
        if (::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0)
        {
            
        }
        return peeraddr;
    }
}