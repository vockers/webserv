#pragma once

#include <cstdint>
#include <functional>
#include <memory>

namespace webserv::async
{
class IPromise;

enum class Poll
{
    READY,
    PENDING,
};

class Event
{
public:
    using PollFn = std::function<Poll()>;

    enum Type
    {
        READABLE = 1 << 0,
        WRITABLE = 1 << 1,
    };

    Event(int fd, uint32_t type, std::unique_ptr<IPromise> promise);

    uint32_t to_epoll() const;
    int      get_fd() const;

    /// Polls the event to check if it is ready
    ///
    /// @return The poll result, either READY or PENDING
    Poll poll() const;

private:
    int      _fd;
    uint32_t _type;
    PollFn   _poll;

    std::unique_ptr<IPromise> _promise;
};
}  // namespace webserv::async
