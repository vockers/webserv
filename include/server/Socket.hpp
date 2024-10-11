#pragma once

#include <vector>

#include "async/Promise.hpp"
#include "server/Address.hpp"

namespace webserv::server
{
using async::Promise;

/// A wrapper around a socket file descriptor.
class Socket
{
public:
    Socket(Address address);
    Socket(Address address, int fd);
    virtual ~Socket();

    Socket(const Socket&)            = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&& other);

    int     get_fd() const;
    Address get_address() const;

    /// Assigns the address specified by `address` to the socket
    ///
    /// @param address The address to bind to the socket
    void bind(Address address);

    Promise<ssize_t> read(std::vector<char>& buffer);
    Promise<ssize_t> write(const std::vector<char>& buffer);

protected:
    Address _address;
    int     _fd;
};
}  // namespace webserv::server
