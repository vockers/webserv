#pragma once

#include <memory>
#include <string>
#include <vector>

#include "server/EventHandler.hpp"
#include "server/Listen.hpp"
#include "utils/Logger.hpp"
#include "server/Client.hpp"

namespace webserv::server
{
using utils::ErrorLogger;

/// A single-threaded non-blocking web server.
class Server
{
public:
    using Events = std::vector<std::unique_ptr<EventHandler>>;

    Server(const std::string& name, const std::string& address, int port, ErrorLogger& elog);
    ~Server();

    /// Runs the event loop.
    void run();

private:
    std::string  _name;
    Listen       _listen;
    int          _epoll_fd;
    ErrorLogger& _elog;

    std::vector<Client> _clients;
    Events              _events;

    void accept();
};
}  // namespace webserv::server
