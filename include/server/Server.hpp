#pragma once

#include <memory>
#include <string>
#include <vector>

#include "config/Config.hpp"
#include "server/Client.hpp"
#include "server/Listen.hpp"
#include "utils/Logger.hpp"

namespace webserv::server
{
using config::Config;
using utils::ErrorLogger;

/// A single-threaded non-blocking web server.
class Server
{
public:
    using Clients = std::vector<std::unique_ptr<Client>>;

    Server(const Config& config, ErrorLogger& elog);

    /// Runs the event loop.
    void run();

    const Config& get_config() const;

private:
    const Config& _config;
    ErrorLogger&  _elog;

    std::string _name;
    Listen      _listen;
    Clients     _clients;
};
}  // namespace webserv::server
