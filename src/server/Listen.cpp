#include "server/Listen.hpp"

#include <fcntl.h>
#include <sys/socket.h>

#include <stdexcept>

namespace webserv::server
{
using async::Event;

Listen::Listen(Address address) : Socket(address)
{
    if (listen(_fd, 16) == -1) {
        throw std::runtime_error("Failed to listen on socket");
    }
}

Promise<Socket> Listen::accept()
{
    return Promise<Socket>(
        [this]() -> std::optional<Socket> {
            sockaddr_in accepted_addr;
            socklen_t   addr_len = sizeof(accepted_addr);

            int fd = ::accept(_fd, (sockaddr*)&accepted_addr, &addr_len);
            if (fd == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    return std::nullopt;
                }
                throw std::runtime_error("Failed to accept connection");
            }
            if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
                throw std::runtime_error("Failed to set O_NONBLOCK on accepted socket");
            }
            return Socket(Socket(Address(accepted_addr), fd));
    }, _fd, Event::READABLE);
}
}  // namespace webserv::server
