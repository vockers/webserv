#include "server/Server.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <algorithm>
#include <memory>

#include "async/Poller.hpp"
#include "utils/Logger.hpp"

namespace webserv::server
{
using async::Poller;
using webserv::utils::ErrorLogger;

Server::Server(const std::string& name, const std::string& address, int port, ErrorLogger& elog)
    : _name(name), _listen(Address(address, port)), _elog(elog)
{
    _elog.log(ErrorLogger::INFO, "Listening on " + _listen.get_address().to_string());
}

void Server::run()
{
    while (true) {
        _listen.accept().then([this](Socket socket) {
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
                                          return client->get_fd() == -1;
                                      }),
                       _clients.end());

        Poller::instance().poll();
    }
}
}  // namespace webserv::server
