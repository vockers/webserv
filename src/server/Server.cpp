#include "server/Server.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <iostream>
#include <stdexcept>

namespace webserv::server
{
Server::Server(const std::string& name, const std::string& address, int port)
    : _name(name), _listen(Address(address, port))
{
    std::cout << "Listening on " << _listen.get_address().to_string() << "\n";

    _epoll_fd = epoll_create(1);
    if (_epoll_fd == -1) {
        throw std::runtime_error("Failed to create epoll instance");
    }
    if (fcntl(_epoll_fd, F_SETFD, FD_CLOEXEC) == -1) {
        throw std::runtime_error("Failed to set FD_CLOEXEC on epoll instance");
    }

    epoll_event event;
    event.events  = EPOLLIN;
    event.data.fd = _listen.get_fd();
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _listen.get_fd(), &event) == -1) {
        throw std::runtime_error("Failed to add listen socket to epoll");
    }
}

Server::~Server()
{
    close(_epoll_fd);
}

void Server::run()
{
    while (true) {
        epoll_event events[10];
        int         num_events = epoll_wait(_epoll_fd, events, 10, -1);
        if (num_events == -1) {
            throw std::runtime_error("Failed to wait for epoll events");
        }

        for (int i = 0; i < num_events; i++) {
            if (events[i].data.fd == _listen.get_fd()) {
                Socket client = _listen.accept();
                _sockets.push_back(std::move(client));
                std::cout << "Accepted connection from " << client.get_address().to_string()
                          << "\n";
            } else {
                // TODO: Handle client events
            }
        }
    }
}
}  // namespace webserv::server
