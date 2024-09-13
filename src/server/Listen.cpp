#include "server/Listen.hpp"

#include <fcntl.h>
#include <sys/socket.h>

#include <stdexcept>

namespace webserv::server
{
Listen::Listen(Address address) : Socket(address)
{
    if (listen(_fd, 16) == -1) {
        throw std::runtime_error("Failed to listen on socket");
    }
}

Socket Listen::accept()
{
    sockaddr_in accepted_addr;
    socklen_t   addr_len = sizeof(accepted_addr);

    int fd = ::accept(_fd, (sockaddr*)&accepted_addr, &addr_len);
    if (fd == -1) {
        throw std::runtime_error("Failed to accept connection");
    }
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
        throw std::runtime_error("Failed to set O_NONBLOCK on accepted socket");
    }

    return Socket(Address(accepted_addr), fd);
}
}  // namespace webserv::server
