#include "async/Event.hpp"

#include <sys/epoll.h>

namespace webserv::async
{
Event::Event(int fd, uint32_t type, PollFn poll)
    : _fd(fd), _type(type), _poll(poll)
{
}

uint32_t Event::to_epoll() const
{
    uint32_t type = EPOLLET;

    if (_type & READABLE)
        type |= EPOLLIN;
    if (_type & WRITABLE)
        type |= EPOLLOUT;

    return type;
}

int Event::get_fd() const
{
    return _fd;
}

Poll Event::poll() const
{
    return _poll();
}
}
