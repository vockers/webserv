#pragma once

#include <sstream>

namespace webserv::server
{
enum class FDStatus
{
    IDLE,
    POLLING,
    DONE,
};

class Pollable
{
public:
    Pollable();
    Pollable(int fd, FDStatus state);

    virtual ~Pollable() = default;

    virtual void poll() = 0;

    int      get_fd() const;
    ssize_t  get_bytes() const;
    FDStatus get_state() const;

    void set_fd(int fd);
    void set_bytes(ssize_t bytes);
    void set_state(FDStatus state);

    std::stringstream& buffer();

private:
    int      _fd;
    ssize_t  _bytes;
    FDStatus _state;

    std::stringstream _buffer;
};

class Readable : public Pollable
{
public:
    using Pollable::Pollable;
    Readable(int fd, FDStatus state);

    ssize_t read();

    void poll();

    virtual void handle_read() = 0;
};

class Writable : public Pollable
{
public:
    using Pollable::Pollable;
    Writable(int fd, FDStatus state);

    ssize_t write();

    void poll();

    virtual void handle_write() = 0;
};
}  // namespace webserv::server
