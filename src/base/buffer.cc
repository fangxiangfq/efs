#include "buffer.h"
#include <unistd.h>

namespace Buffer
{
    ssize_t Buffer::recv(int fd, int* savedErrno, size_t len)
    {
        ensureWritableBytes(len);
        const ssize_t n = ::read(fd, beginWrite(), len);

        if (n < 0)
        {
            *savedErrno = errno;
        }
        else
        {
            writerIndex_ += n;
        }

        return n;
    }

    ssize_t Buffer::send(int fd, int* savedErrno, size_t len)
    {
        const size_t readable = readableBytes();
        if(len > readable)
        {
            len = readable;
        }

        //todo loop send
        const ssize_t n = ::write(fd, peek(), len);

        if (n < 0)
        {
            *savedErrno = errno;
        }

        return n;
    }

    ssize_t Buffer::send(int fd, int* savedErrno)
    {
        return send(fd, savedErrno, readableBytes());
    }

    ssize_t Buffer::sendto(const Socket::SockInfo& info, int* savedErrno)
    {
        const ssize_t n = ::sendto(info.sockfd_, peek(), readableBytes(), 0, info.peerAddr_.getSockAddr(), sizeof(struct sockaddr));

        if (n < 0)
        {
            *savedErrno = errno;
        }

        return n;
    }
}
