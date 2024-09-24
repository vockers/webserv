#include "server/Server.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

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

    auto event_handler     = std::make_unique<EventHandler>();
    event_handler->handle_read = std::bind(&Server::accept, this);

    // Add the listen socket to the epoll instance.
    epoll_event event;
    event.events   = EPOLLIN;
    event.data.fd  = _listen.get_fd();
    event.data.ptr = event_handler.get();
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _listen.get_fd(), &event) == -1) {
        _events.pop_back();
        throw std::runtime_error("Failed to add listen socket to epoll");
    }

    _events.push_back(std::move(event_handler));
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
            const EventHandler* handler = static_cast<EventHandler*>(events[i].data.ptr);

            if (events[i].events & EPOLLIN) {
                handler->handle_read();
            }
            if (events[i].events & EPOLLOUT) {
                handler->handle_write();
            }
        }
    }
}

void Server::accept()
{
    Socket socket = _listen.accept();
	_clients.emplace_back(std::make_unique<Client>(std::move(socket)));

    Client& client = *(_clients.back());

    auto event_handler     = std::make_unique<EventHandler>();
    event_handler->handle_read = std::bind(&Client::handle_read, &client);
    event_handler->handle_write = std::bind(&Client::handle_write, &client);

    // Add the listen socket to the epoll instance.
    epoll_event event;
    event.events   = EPOLLIN | EPOLLOUT | EPOLLET;
    event.data.fd  = client.get_fd();
    event.data.ptr = event_handler.get();
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client.get_fd(), &event) == -1) {
        _events.pop_back();
        throw std::runtime_error("Failed to add client socket with fd: " + std::to_string(socket.get_fd()) + " to epoll");
    }

    _events.push_back(std::move(event_handler));

    _elog.log(ErrorLogger::INFO, "Accepted connection from " + socket.get_address().to_string());
}
}  // namespace webserv::server
