#pragma once

#include "server/Socket.hpp"

/// A socket that listens for incoming connections.
class Listen : public Socket
{
public:
    Listen(Address address);

    Listen(const Listen&)            = delete;
    Listen& operator=(const Listen&) = delete;

    /// Accept a new connection.
    ///
    /// Extracts the first connection request on the queue
    /// of pending connections for the  listening  socket
    ///
    /// @return Socket of the accepted connection.
    Socket accept();
};
