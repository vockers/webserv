#pragma once

#include <netinet/in.h>

class Address
{
public:
    Address(uint32_t address, int port);

    sockaddr_in& get_sockaddr();

private:
    sockaddr_in _addr;
};
    
