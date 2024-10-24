#pragma once

#include <map>
#include <memory>

#include "config/Config.hpp"
#include "net/VirtualServer.hpp"
#include "utils/Logger.hpp"

namespace webserv::net
{
using config::Config;
using utils::ErrorLogger;

/// A single-threaded non-blocking web server.
class Server
{
public:
    using VirtualServers = std::map<int, std::unique_ptr<VirtualServer>>;

    Server(const Config& config, ErrorLogger& elog);

    /// Runs the event loop.
    void run();

    /// @brief Returns the http directive configuration.
    const Config& get_config() const;

private:
    const Config& _config;
    ErrorLogger&  _elog;

    VirtualServers _virtual_servers;
};
}  // namespace webserv::net
