#include "server/Address.hpp"

#include <arpa/inet.h>

Address::Address(sockaddr_in addr) : _addr(addr), _port(ntohs(addr.sin_port)) {}

Address::Address(uint32_t address, int port) : _port(port)
{
    _addr.sin_family      = AF_INET;
    _addr.sin_addr.s_addr = address;
    _addr.sin_port        = htons(port);
}

Address::Address(const std::string& address, int port) : Address(inet_addr(address.c_str()), port)
{
}

sockaddr_in& Address::get_sockaddr()
{
    return _addr;
}

int Address::get_port() const
{
    return _port;
}

std::string Address::to_string() const
{
    char buffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &_addr.sin_addr, buffer, INET_ADDRSTRLEN);
    return std::string(buffer) + ":" + std::to_string(_port);
}
