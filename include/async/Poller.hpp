#pragma once

#include <sys/epoll.h>

#include <memory>
#include <unordered_map>

#include "async/Event.hpp"

namespace webserv::async
{
class IPromise;

class Poller
{
public:
    using Events   = std::unordered_map<int, std::unique_ptr<Event>>;
    using Promises = std::vector<std::unique_ptr<IPromise>>;

    Poller();
    ~Poller();

    void poll();

    void add_promise(std::unique_ptr<IPromise> promise, int fd, Event::Type type);
    void add_promise(std::unique_ptr<IPromise> promise);
    void add_event(Event event);

    static Poller& instance();

private:
    int      _epoll_fd;
    Events   _events;
    Promises _blocking_promises;
};
}  // namespace webserv::async
