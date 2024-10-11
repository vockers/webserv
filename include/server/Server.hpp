#pragma once

#include <memory>
#include <string>
#include <vector>

#include "server/Client.hpp"
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

    Server(const std::string& name, const std::string& address, int port, ErrorLogger& elog);

    /// Runs the event loop.
    void run();

private:
    std::string  _name;
    Listen       _listen;
    ErrorLogger& _elog;

    Clients _clients;

    void accept();
};
}  // namespace webserv::server
