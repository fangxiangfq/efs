#pragma once
#include <vector>
#include <string>
#include <assert.h>
#include <sys/socket.h>
#include <string.h>
#include "sockets.h"
namespace Buffer
{
    class Buffer
    {
    public:
        static const size_t kInitialSize = 2048;
        static const size_t kCheapPrepend = 8;
        explicit Buffer(size_t initialSize = kInitialSize)
        : buffer_(kCheapPrepend + initialSize),
          readerIndex_(kCheapPrepend),
          writerIndex_(kCheapPrepend){}

        void swap(Buffer& rhs)
        {
            buffer_.swap(rhs.buffer_);
            std::swap(readerIndex_, rhs.readerIndex_);
            std::swap(writerIndex_, rhs.writerIndex_);
        }

        size_t readableBytes() const
        { return writerIndex_ - readerIndex_; }

        size_t writableBytes() const
        { return buffer_.size() - writerIndex_; }

        size_t prependableBytes() const
        { return readerIndex_; }

        const char* peek() const
        { return begin() + readerIndex_; }

        void retrieveAll()
        {
            readerIndex_ = kCheapPrepend;
            writerIndex_ = kCheapPrepend;
        }

        void retrieve(size_t len)
        {
            assert(len <= readableBytes());
            if (len < readableBytes())
            {
                readerIndex_ += len;
            }
            else
            {
                retrieveAll();
            }
        }

        void retrieveUntil(const char* end)
        {
            assert(peek() <= end);
            assert(end <= beginWrite());
            retrieve(end - peek());
        }
        
        std::string retrieveAsString(size_t len)
        {
            assert(len <= readableBytes());
            std::string ret(peek(), len);
            retrieve(len);
            return ret;
        }

        void retrieveAsStringAll(std::string& str)
        {
            str.assign(peek(), readableBytes());
            retrieve(readableBytes());
        }

        void prepend(const void* /*restrict*/ data, size_t len)
        {
            assert(len <= prependableBytes());
            readerIndex_ -= len;
            const char* d = static_cast<const char*>(data);
            std::copy(d, d+len, begin()+readerIndex_);
        }

        char* beginWrite()
        { return begin() + writerIndex_; }

        const char* readEnd() const
        { return begin() + writerIndex_; }

        void hasWritten(size_t len)
        {
            assert(len <= writableBytes());
            writerIndex_ += len;
        } 

        void makeSpace(size_t len)
        {
            if (writableBytes() + prependableBytes() < len + kCheapPrepend)
            {
                buffer_.resize(writerIndex_+len);
            }
        }

        void ensureWritableBytes(size_t len)
        {
            if (writableBytes() < len)
            {
                makeSpace(len);
            }
            assert(writableBytes() >= len);
        }

        void append(const std::string& str)
        {
            append(str.c_str(), str.length());
        }

        void append(const char* /*restrict*/ data, size_t len)
        {
            ensureWritableBytes(len);
            std::copy(data, data+len, beginWrite());
            hasWritten(len);
        }

        void append(const void* /*restrict*/ data, size_t len)
        {
            append(static_cast<const char*>(data), len);
        }

        ssize_t recv(int fd, int* savedErrno, size_t len);
        ssize_t send(int fd, int* savedErrno, size_t len);
        ssize_t send(int fd, int* savedErrno);
        ssize_t sendto(const Socket::SockInfo& info, int* savedErrno);
        ssize_t fread(int fd, int* savedErrno, size_t len) { return recv(fd, savedErrno, len); }
        //only support basic type without type cast
        template <typename T>
        T read() 
        { 
            assert(readableBytes() >= sizeof(T));
            T ret;
            ::memcpy(&ret, peek(), sizeof(T));
            retrieve(sizeof(T));
            return ret;
        }

        template <typename T>
        T nread()  // network byte order read
        {
            assert(readableBytes() >= sizeof(T));
            T ret;
            void* pv = &ret;
            char* pc = static_cast<char*>(pv);
            size_t size = sizeof(T);
            for(size_t i = size; i > 0; --i){
                ::memcpy(pc + i - 1, peek(), 1);
                retrieve(1);
            }
            
            return ret;
        }

        //only support basic type without type cast
        template <typename T>
        void write(const T& x) 
        { 
            append(&x, sizeof(T));
        }

        //only support basic type without type cast
        template <typename T>
        void prewrite(const T& x) 
        { 
            prepend(&x, sizeof(T));
        }

    private:
        char* begin()
        { return &*buffer_.begin(); }

        const char* begin() const
        { return &*buffer_.begin(); }
        std::vector<char> buffer_;
        size_t readerIndex_;
        size_t writerIndex_;
    };
}

