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

    /// Poll for events and promises
    void poll();

    /// Adds a promise to the poller with the specified
    /// file descriptor and event type
    ///
    /// @param promise The promise to add
    /// @param fd The file descriptor to poll
    /// @param type The event type to poll for
    void add_promise(std::unique_ptr<IPromise> promise, int fd, Event::Type type);

    /// Adds a promise to the poller
    ///
    /// @param promise The promise to add
    void add_promise(std::unique_ptr<IPromise> promise);

    static Poller& instance();

private:
    int      _epoll_fd;
    Events   _events;
    Promises _blocking_promises;
};
}  // namespace webserv::async
