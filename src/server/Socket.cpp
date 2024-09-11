#include "server/Socket.hpp"

#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

Socket::Socket(Address address) : _addr(address)
{
    _fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (_fd == -1) {
        throw std::runtime_error("Failed to create socket");
    }

    this->bind();
}

Socket::Socket(Address address, int fd) : _addr(address), _fd(fd)
{
}

Socket::~Socket()
{
    close(_fd);
}

Socket::Socket(Socket&& other) : _addr(other._addr), _fd(other._fd)
{
    other._fd = -1;
}

int Socket::get_fd() const
{
    return _fd;
}

Address Socket::get_address() const
{
    return _addr;
}

void Socket::bind()
{
    const int reuse = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) != 0) {
        throw std::runtime_error("Failed to set SO_REUSEADDR");
	}
    if (::bind(_fd, (sockaddr*)&_addr.get_sockaddr(), sizeof(sockaddr_in)) == -1) {
        throw std::runtime_error("Failed to bind socket");
    }
}

