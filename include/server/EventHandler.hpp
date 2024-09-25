#pragma once

#include <functional>

namespace webserv::server
{
struct EventHandler
{
    std::function<void()> handle_read;
    std::function<void()> handle_write;

    int fd;
};
}  // namespace webserv::server
