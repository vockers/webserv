#pragma once

#include <sys/epoll.h>
#include <unordered_map>
#include <memory>

#include "async/Event.hpp"

namespace webserv::async
{
class IPromise;

class Poller
{
public:
    using Events = std::unordered_map<int, std::unique_ptr<Event>>;
    using Promises = std::unordered_map<int, std::unique_ptr<IPromise>>;

    Poller();
    ~Poller();

    void poll();

    void add_promise(IPromise promise, int fd, Event::Type type);

    static Poller& instance();

private:
    int    _epoll_fd;
    Events _events;
    Promises _promises;
};
}
