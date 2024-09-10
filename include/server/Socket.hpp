#pragma once

#include "server/Address.hpp"

class Socket
{
public:
    Socket(uint32_t address, int port);
    ~Socket();

    int  get_fd() const;
    void bind();

protected:
    Address _addr;
    int     _fd;
};
