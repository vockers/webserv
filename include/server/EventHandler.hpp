#pragma once

#include <cstdint>
#include <functional>

namespace webserv::server
{
class EventHandler
{
public:
    using Handler = std::function<void()>;

    EventHandler(int fd, Handler read_handler = nullptr, Handler write_handler = nullptr);

    void handle_read() const;
    void handle_write() const;

    int      get_fd() const;
    uint32_t get_type() const;

private:
    std::function<void()> _read_handler;
    std::function<void()> _write_handler;

    int      _fd;
    uint32_t _type;
};
}  // namespace webserv::server
