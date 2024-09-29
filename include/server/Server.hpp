#pragma once

#include <memory>
#include <string>
#include <vector>

#include "server/Client.hpp"
#include "server/EventHandler.hpp"
#include "server/Listen.hpp"
#include "utils/Logger.hpp"

namespace webserv::server
{
using utils::ErrorLogger;

/// A single-threaded non-blocking web server.
class Server
{
public:
    using Clients = std::vector<std::unique_ptr<Client>>;
    using Events  = std::vector<std::unique_ptr<EventHandler>>;

    Server(const std::string& name, const std::string& address, int port, ErrorLogger& elog);
    ~Server();

    /// Runs the event loop.
    void run();

    void add_event(const EventHandler& event);
    void add_blocking_event(const EventHandler& event);

private:
    std::string  _name;
    Listen       _listen;
    int          _epoll_fd;
    ErrorLogger& _elog;

    Clients _clients;

    Events _events;
    Events _blocking_events;

    void accept();
};
}  // namespace webserv::server
