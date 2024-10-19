#pragma once

#include <netinet/in.h>

#include <string>

namespace webserv::net
{
/// Represents an IP address and port.
class Address
{
public:
    Address();
    Address(sockaddr_in addr);
    Address(uint32_t address, int port);
    Address(const std::string& address, int port);

    sockaddr_in& get_sockaddr();
    int          get_port() const;

    /// Returns a string representation of the address
    /// in the format "address:port".
    std::string to_string() const;

private:
    sockaddr_in _addr;
    int         _port;
};
}  // namespace webserv::net
