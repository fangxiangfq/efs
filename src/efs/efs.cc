#include "efs.h"
#include "inetaddress.h"

Efs::Efs() 
:main_loop_(), server_(&main_loop_, std::string("efs"))
{
    server_.setEvCallback(std::bind(&Efs::dispatch, this, _1));
}

void Efs::registerTasks(Event::TaskMap& taskmap) 
{
    assert(!started_);
    taskMap_.swap(taskmap);
}

void Efs::registerTasks(const short& task, const Event::TaskCb& cb) 
{
    assert(!started_);
    taskMap_.emplace(task, cb);
}

void Efs::dispatch(Event::Event& ev) 
{
    if(ev.ownerLoop() != &main_loop_)
    {
        //todo log
        abort();
    }

    switch (ev.type())
    {
    case Event::EvType::udp:
        onUdpMessage(ev);
        break;
    case Event::EvType::local:
        onLocalMessage(ev);
        break;
    case Event::EvType::tcplink:
        onTcpLink(ev);
        break;
    case Event::EvType::tcp:
        onTcpMessage(ev);
        break;
    default:
        break;
    }
}

void Efs::taskPost(Buffer::Buffer& buf) 
{
    int savedErrno = 0;
    int workerFd = server_.getNextWorkerFd();
    assert(workerFd > 0);
    if(0 > buf.send(workerFd, &savedErrno))
    {
        //todo log
    }
}

void Efs::onRequest(Buffer::Buffer& buf) 
{
    Http::HttpContext context;
    if(!context.parseRequest(buf))
    {
        
    }
}

void Efs::onTcpLink(Event::Event& ev) 
{
    int fd = ev.fd();
    struct sockaddr_in addr;
    socklen_t addrlen = static_cast<socklen_t>(sizeof addr);
    int connfd = ::accept(fd, Net::sockaddr_cast(&addr), &addrlen);
    server_.createChan(fd);
}

void Efs::onTcpMessage(Event::Event& ev) 
{
    int savedErrno = 0;
    Buffer::Buffer buf(256);
    int fd = ev.fd();
    ssize_t len = buf.recv(fd, &savedErrno, 256);
    onRequest(buf);
}

void Efs::onUdpMessage(Event::Event& ev) 
{
    Buffer::Buffer buf;
    int fd = ev.fd();
    size_t len = 0;
    uint16_t task = static_cast<uint16_t>(Task::udpforward);
    buf.write(task);
    len += sizeof(task);
    buf.write(fd);
    len += sizeof(fd);

    setTaskHeader(buf, len);
    ev.disableAll();
    taskPost(buf);
}

void Efs::onLocalMessage(Event::Event& ev) 
{
    Buffer::Buffer buf;
    int fd = ev.fd();
    int savedErrno = 0;

    while(true)
    {
        ssize_t len = buf.recv(fd, &savedErrno, 4);

        if(0 > len)
        {
            if((savedErrno == EAGAIN || savedErrno == EWOULDBLOCK))
            {
                break;
            }  
        } 
        else if(0 == len)
        {
            //todo log
            ev.disableAll();
            break;
        }

        uint32_t leftLen = 0;
        buf.read(leftLen);
        len = buf.recv(fd, &savedErrno, static_cast<size_t>(leftLen));

        if(0 > len)
        {
            if((savedErrno == EAGAIN || savedErrno == EWOULDBLOCK))
            {
                break;
            }  
        } 
        else if(0 == len || len > static_cast<ssize_t>(leftLen))
        {
            //todo log
            ev.disableAll();
            break;
        }

    }

    uint16_t task = 0;

    if(true == checkTaskHeader(buf, task))
    {
        Event::TaskCb cb = taskMap_[task];
        if(cb)
        {
            // cb(buf);
        }
    }
    else
    {
        //todo log
    }
}

void Efs::setTaskHeader(Buffer::Buffer& buf, size_t len)
{
    assert(buf.prependableBytes() >= 8);
    uint32_t len32 = static_cast<uint32_t>(len);
    buf.prewrite('k');
    buf.prewrite('s');
    buf.prewrite('a');
    buf.prewrite('t');
    len32 += 4;
    buf.prewrite(len32);
}

bool Efs::checkTaskHeader(Buffer::Buffer& buf, uint16_t& task)
{
    assert(buf.readableBytes() < 8);
    char ch;
    buf.read(ch);
    if(ch != 't')
        return false;
    buf.read(ch);
    if(ch != 'a')
        return false;
    buf.read(ch);
    if(ch != 's')
        return false;
    buf.read(ch);
    if(ch != 'k')
        return false;
    buf.read(task);
    return true;
}

