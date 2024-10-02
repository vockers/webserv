#pragma once

#include <unordered_map>
#include <memory>

#include "async/Event.hpp"

namespace webserv::async
{
class Poller
{
public:
    using Events = std::unordered_map<int, std::unique_ptr<Event>>;

    Poller();
    ~Poller();

    void poll();

    void add(Event event);

    static Poller& instance();

private:
    int    _epoll_fd;
    Events _events;
};
}
