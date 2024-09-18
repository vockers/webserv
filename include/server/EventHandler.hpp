#pragma once

#include <functional>

namespace webserv::server
{
struct EventHandler
{
    std::function<void()> on_read;
    std::function<void()> on_write;
};
}  // namespace webserv::server
