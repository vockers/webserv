#pragma once

#include <sstream>

#include "http/Request.hpp"
#include "server/Socket.hpp"
#include "utils/Logger.hpp"

namespace webserv::server
{
using webserv::http::Request;
using webserv::utils::ErrorLogger;

enum class EStatus
{
    IDLE,
    POLLING,
    DONE,
};

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

    void read();
    void write();

    Request _request;

    EStatus _read_state;
    EStatus _write_state;
};
}  // namespace webserv::server
