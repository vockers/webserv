#include "server/Socket.hpp"

#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

Socket::Socket(uint32_t address, int port) : _addr(address, port)
{
    _fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (_fd == -1) {
        throw std::runtime_error("Failed to create socket");
    }

    this->bind();
}

Socket::~Socket()
{
    close(_fd);
}

int Socket::get_fd() const
{
    return _fd;
}

void Socket::bind()
{
    if (::bind(_fd, (sockaddr*)&_addr.get_sockaddr(), sizeof(sockaddr_in)) == -1) {
        throw std::runtime_error("Failed to bind socket");
    }
}

