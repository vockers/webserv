#include "server/Listen.hpp"

#include <stdexcept>
#include <sys/socket.h>

Listen::Listen(uint32_t address, int port) : Socket(address, port)
{
    if (listen(_fd, 16) == -1) {
        throw std::runtime_error("Failed to listen on socket");
    }
}
