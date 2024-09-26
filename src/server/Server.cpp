#include "server/Server.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "server/EventHandler.hpp"
#include "utils/Logger.hpp"

namespace webserv::server
{
using webserv::utils::ErrorLogger;

Server::Server(const std::string& name, const std::string& address, int port, ErrorLogger& elog)
    : _name(name), _listen(Address(address, port)), _elog(elog)
{
    _elog.log(ErrorLogger::INFO, "Listening on " + _listen.get_address().to_string());

    _epoll_fd = epoll_create(1);
    if (_epoll_fd == -1) {
        throw std::runtime_error("Failed to create epoll instance");
    }
    if (fcntl(_epoll_fd, F_SETFD, FD_CLOEXEC) == -1) {
        throw std::runtime_error("Failed to set FD_CLOEXEC on epoll instance");
    }

    // Add the listen socket to the event loop.
    EventHandler event_handler(_listen.get_fd(), std::bind(&Server::accept, this));
    this->add_event(event_handler);
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
            EventHandler* handler = static_cast<EventHandler*>(events[i].data.ptr);
            if (events[i].events & EPOLLIN) {
                handler->handle_read();
            }
            if (events[i].events & EPOLLOUT) {
                handler->handle_write();
            }
        }
    }
}

void Server::add_event(const EventHandler& event)
{
    auto event_ptr = std::make_unique<EventHandler>(std::move(event));

    epoll_event epoll_event;
    epoll_event.events   = static_cast<uint32_t>(event_ptr->get_type()) | EPOLLET;
    epoll_event.data.ptr = event_ptr.get();

    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, event_ptr->get_fd(), &epoll_event) == -1) {
        throw std::runtime_error("Failed to add event to epoll");
    }

    _events.push_back(std::move(event_ptr));
}

void Server::accept()
{
    Socket socket = _listen.accept();
    _clients.emplace_back(std::make_unique<Client>(std::move(socket), _elog));

    Client& client = *(_clients.back());

    EventHandler event(client.get_fd(),
                       std::bind(&Client::handle_read, &client),
                       std::bind(&Client::handle_write, &client));
    this->add_event(event);

    _elog.log(ErrorLogger::INFO, "Accepted connection from " + socket.get_address().to_string());
}
}  // namespace webserv::server
