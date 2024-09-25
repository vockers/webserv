#pragma once

#include <sstream>

#include "server/Socket.hpp"
#include "utils/Logger.hpp"

namespace webserv::server
{
using webserv::utils::ErrorLogger;

class Client : public Socket
{
public:
    Client(Socket&& socket, ErrorLogger& elog);
    ~Client();

    void handle_read();
    void handle_write();

private:
    std::stringstream _buffer;
    ErrorLogger&      _elog;
};
}  // namespace webserv::server