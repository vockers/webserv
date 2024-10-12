#include "async/Event.hpp"

#include <sys/epoll.h>

#include <functional>

#include "async/Promise.hpp"

namespace webserv::async
{
Event::Event(int fd, uint32_t type, std::unique_ptr<IPromise> promise)
    : _fd(fd), _type(type), _promise(std::move(promise))
{
    _poll = std::bind(&IPromise::poll, _promise.get());
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
}  // namespace webserv::async
