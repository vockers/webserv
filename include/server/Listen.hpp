#pragma once

#include "server/Socket.hpp"

/// A socket that listens for incoming connections.
class Listen : public Socket
{
public:
    Listen(Address address);

    Listen(const Listen&)            = delete;
    Listen& operator=(const Listen&) = delete;

    Socket accept();
};
