#include "net/Server.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <memory>

#include "async/Poller.hpp"

namespace webserv::net
{
using async::Poller;

Server::Server(const Config& config, ErrorLogger& elog) : _config(config), _elog(elog)
{
    for (auto it = config.begin(Config::Type::SERVER); it != config.end();
         it      = it.next(Config::Type::SERVER)) {
        if (_virtual_servers.find(it->port()) == _virtual_servers.end()) {
            _virtual_servers[it->port()] = std::make_unique<VirtualServer>(
                Address(it->host(), it->port()), it->server_name(), elog);
        }
        _virtual_servers[it->port()]->add_config(*it);
    }
}

void Server::run()
{
    while (true) {
        for (const auto& server : _virtual_servers) {
            server.second->listen();
        }

        Poller::instance().poll();
    }
}

const Config& Server::get_config() const
{
    return _config;
}
}  // namespace webserv::net
