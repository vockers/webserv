#include "server/Pollable.hpp"

#include <unistd.h>
#include <memory>

#define BUFFER_SIZE 4096

namespace webserv::server
{
Pollable::Pollable()
    : _fd(-1)
    , _state(FDStatus::IDLE)
{
}

Pollable::Pollable(int fd, FDStatus state)
    : _fd(fd)
    , _state(state)
{
}

int Pollable::get_fd() const
{
    return _fd;
}

ssize_t Pollable::get_bytes() const
{
    return _bytes;
}

FDStatus Pollable::get_state() const
{
    return _state;
}

void Pollable::set_fd(int fd)
{
    _fd = fd;
}

void Pollable::set_bytes(ssize_t bytes)
{
    _bytes = bytes;
}

void Pollable::set_state(FDStatus state)
{
    _state = state;
}

Readable::Readable(int fd, FDStatus state)
    : Pollable(fd, state)
{
}

ssize_t Readable::read()
{
    std::unique_ptr<char[]> buffer(new char[BUFFER_SIZE]{0});

    ssize_t bytes_read = ::read(get_fd(), buffer.get(), BUFFER_SIZE);
    this->write(buffer.get(), bytes_read);
    set_bytes(get_bytes() + bytes_read);
    return bytes_read;
}

void Readable::poll()
{
    if (get_state() == FDStatus::POLLING)
        handle_read();
}

Writable::Writable(int fd, FDStatus state)
    : Pollable(fd, state)
{
}

ssize_t Writable::write()
{
    ssize_t bytes_written = ::write(get_fd(), str().data(), str().size());
    set_bytes(get_bytes() + bytes_written);
    return bytes_written;
}

void Writable::poll()
{
    if (get_state() == FDStatus::POLLING)
        handle_write();
}
}
