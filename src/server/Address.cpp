#include "server/Address.hpp"

Address::Address(uint32_t address, int port)
{
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = htonl(address);
    _addr.sin_port = htons(port);
}

sockaddr_in& Address::get_sockaddr()
{
    return _addr;
}
