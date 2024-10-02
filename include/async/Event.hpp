#pragma once

#include <cstdint>
#include <functional>

namespace webserv::async
{
class Event
{
public:
    using PollFn = std::function<void()>;

    enum Type
    {
        READABLE = 1 << 0,
        WRITABLE = 1 << 1,
    }; 

    Event(int fd, uint32_t type, PollFn poll);

    uint32_t to_epoll() const;
    int      get_fd() const;

    void poll() const;

private:
    int      _fd;
    uint32_t _type;
    PollFn   _poll;
};
}
