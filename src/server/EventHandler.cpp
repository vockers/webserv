#include "server/EventHandler.hpp"

#include <sys/epoll.h>

namespace webserv::server
{
EventHandler::EventHandler(int fd, Handler read_handler, Handler write_handler)
    : _read_handler(read_handler), _write_handler(write_handler), _fd(fd), _type(0)
{
    if (_read_handler != nullptr) {
        _type |= EPOLLIN;
    }
    if (_write_handler != nullptr) {
        _type |= EPOLLOUT;
    }
}

void EventHandler::handle_read() const
{
    if (_read_handler == nullptr) {
        return;
    }

    _read_handler();
}

void EventHandler::handle_write() const
{
    if (_write_handler == nullptr) {
        return;
    }

    _write_handler();
}

int EventHandler::get_fd() const
{
    return _fd;
}

uint32_t EventHandler::get_type() const
{
    return _type;
}
}  // namespace webserv::server
