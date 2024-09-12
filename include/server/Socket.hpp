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

    /// Assigns the address specified by `address` to the socket
    ///
    /// @param address The address to bind to the socket
    void bind(Address address);

protected:
    Address _address;
    int     _fd;
};
