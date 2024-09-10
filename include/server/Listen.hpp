#pragma once

#include "server/Socket.hpp"

class Listen: public Socket
{
public:
    Listen(uint32_t address, int port);

    int accept();
};
