#include "net/VirtualServer.hpp"

namespace webserv::net
{
VirtualServer::VirtualServer(Address address, const std::string& default_name, ErrorLogger& elog)
    : Listen(address), _elog(elog), _default_name(default_name)
{
    _elog.log(ErrorLogger::INFO, "Listening on " + this->get_address().to_string());
}

void VirtualServer::listen()
{
    this->accept().then([this](Socket socket) {
        _clients.emplace_back(std::make_unique<Client>(std::move(socket), *this, _elog));

        Client& client = *(_clients.back());
        _elog.log(ErrorLogger::INFO,
                  "Accepted connection from " + client.get_address().to_string());
        client.handle_connection();
    });

    // Remove disconnected clients
    _clients.erase(std::remove_if(_clients.begin(),
                                  _clients.end(),
                                  [](const std::unique_ptr<Client>& client) {
                                      return client->is_connected() == false;
                                  }),
                   _clients.end());
}

void VirtualServer::add_config(const Config& config)
{
    _configs[config.server_name()] = &config;
}

const Config& VirtualServer::get_config(const std::string& host) const
{
    if (_configs.find(host) == _configs.end()) {
        return *_configs.at(_default_name);
    }

    return *_configs.at(host);
}
}  // namespace webserv::net
