#include "server/Socket.hpp"

#include <sys/socket.h>
#include <unistd.h>

#include <stdexcept>

Socket::Socket(Address address)
{
    _fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (_fd == -1) {
        throw std::runtime_error("Failed to create socket");
    }

    this->bind(address);
}

Socket::Socket(Address address, int fd) : _address(address), _fd(fd) {}

Socket::~Socket()
{
    close(_fd);
}

Socket::Socket(Socket&& other) : _address(other._address), _fd(other._fd)
{
    other._fd = -1;
}

int Socket::get_fd() const
{
    return _fd;
}

Address Socket::get_address() const
{
    return _address;
}

void Socket::bind(Address address)
{
    const int reuse = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) != 0) {
        throw std::runtime_error("Failed to set SO_REUSEADDR");
    }
    if (::bind(_fd, (sockaddr*)&address.get_sockaddr(), sizeof(sockaddr_in)) == -1) {
        throw std::runtime_error("Failed to bind socket");
    }
    _address = address;
}
