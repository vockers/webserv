#include "server/Socket.hpp"

#include <sys/socket.h>
#include <unistd.h>

#include <stdexcept>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif

namespace webserv::server
{
using async::Event;

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
    ::close(_fd);
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

void Socket::close()
{
    ::close(_fd);
    _fd = -1;
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

Promise<ssize_t> Socket::read(std::vector<char>& buffer)
{
    return Promise<ssize_t>(
        [this, &buffer]() -> std::optional<ssize_t> {
            buffer.resize(BUFFER_SIZE);
            ssize_t bytes_read = ::read(_fd, buffer.data(), BUFFER_SIZE);
            if (bytes_read == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    return std::nullopt;
                }
                throw std::runtime_error("Failed to read from socket");
            }
            return bytes_read;
        },
        _fd,
        Event::READABLE);
}

Promise<ssize_t> Socket::write(const std::vector<char>& buffer)
{
    return Promise<ssize_t>(
        [this, &buffer]() -> std::optional<ssize_t> {
            ssize_t bytes_written = ::write(_fd, buffer.data(), buffer.size());
            if (bytes_written == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    return std::nullopt;
                }
                throw std::runtime_error("Failed to write to socket");
            }
            return bytes_written;
        },
        _fd,
        Event::WRITABLE);
}
}  // namespace webserv::server
