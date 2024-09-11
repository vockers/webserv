#pragma once

#include <netinet/in.h>

#include <string>

class Address
{
public:
    Address(sockaddr_in addr);
    Address(uint32_t address, int port);
    Address(const std::string& address, int port);

    sockaddr_in& get_sockaddr();
    int          get_port() const;
    std::string  to_string() const;

private:
    sockaddr_in _addr;
    int         _port;
};
