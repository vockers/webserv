#pragma once

#include <string>
#include <unordered_map>

#include "config/Config.hpp"
#include "net/Client.hpp"
#include "net/Listen.hpp"
#include "utils/Logger.hpp"

namespace webserv::net
{
using config::Config;
using utils::Logger;

class VirtualServer : public Listen
{
public:
    using ConfigMap = std::unordered_map<std::string, const Config*>;
    using Clients   = std::vector<std::unique_ptr<Client>>;

    VirtualServer(Address address, const std::string& default_name, ErrorLogger& elog);

    /// @brief Accepts new connections and adds them to the clients list
    void listen();

    /// @brief Add config to the virtual server
    ///
    /// @param config Config to add
    void add_config(const Config& config);

    /// @brief Get config by host
    ///
    /// @param host Host to get config for
    const Config& get_config(const std::string& host) const;

private:
    ErrorLogger& _elog;

    const std::string& _default_name;

    ConfigMap _configs;
    Clients   _clients;
};
}  // namespace webserv::net
