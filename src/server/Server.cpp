#include "server/Server.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>

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

        Poller::instance().poll();
    }
}

void Server::accept()
{
    /*Socket socket = _listen.accept();*/
    /*_clients.emplace_back(std::make_unique<Client>(std::move(socket), *this, _elog));*/
    /**/
    /*Client& client = *(_clients.back());*/
    /**/
    /*EventHandler event(client.get_fd(),*/
    /*                   std::bind(&Client::Readable::poll, &client),*/
    /*                   std::bind(&Client::Writable::poll, &client));*/
    /*this->add_event(event);*/
    /**/
    /*_elog.log(ErrorLogger::INFO, "Accepted connection from " +
     * socket.get_address().to_string());*/
}
}  // namespace webserv::server
