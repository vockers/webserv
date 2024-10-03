#pragma once

#include <memory>

#include "http/Request.hpp"
/*#include "http/Response.hpp"*/
#include "server/Socket.hpp"
#include "utils/Logger.hpp"

namespace webserv::server
{
using http::Request;
/*using http::Response;*/
using utils::ErrorLogger;

class Server;

class Client : public Socket
{
public:
    Client(Socket&& socket, Server& server, ErrorLogger& elog);
    ~Client();

    using Socket::get_fd;

private:
    Server&      _server;
    ErrorLogger& _elog;

    std::unique_ptr<Request>  _request;
    /*std::unique_ptr<Response> _response;*/
};
}  // namespace webserv::server
