#pragma once

#include <string>
#include <vector>

#include "server/Listen.hpp"

namespace webserv::server
{
/// A single-threaded non-blocking web server.
class Server
{
public:
    Server(const std::string& name, const std::string& address, int port);
    ~Server();

    /// Runs the event loop.
    void run();

private:
    std::string _name;
    Listen      _listen;
    int         _epoll_fd;

    std::vector<Socket> _sockets;
};
}  // namespace webserv::server
