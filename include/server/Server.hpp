#pragma once

#include <string>
#include <vector>

#include "server/Listen.hpp"
#include "utils/Logger.hpp"

namespace webserv::server
{
using utils::ErrorLogger;

/// A single-threaded non-blocking web server.
class Server
{
public:
    Server(const std::string& name, const std::string& address, int port, ErrorLogger& elog);
    ~Server();

    /// Runs the event loop.
    void run();

private:
    std::string  _name;
    Listen       _listen;
    int          _epoll_fd;
    ErrorLogger& _elog;

    std::vector<Socket> _sockets;
};
}  // namespace webserv::server
