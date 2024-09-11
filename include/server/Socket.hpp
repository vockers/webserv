#pragma once

#include "server/Address.hpp"

/// A wrapper around a socket file descriptor.
class Socket
{
public:
    Socket(Address address);
    Socket(Address address, int fd);
    ~Socket();

    Socket(const Socket&)            = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&& other);

    int     get_fd() const;
    Address get_address() const;

    void bind();

protected:
    Address _addr;
    int     _fd;
};
