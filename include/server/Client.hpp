#pragma once

#include "http/Request.hpp"
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

    /// Handles the connection by reading a
    /// request and sending a response
    void handle_connection();

private:
    /// Asynchronously reads a request from the client
    ///
    /// @return The request as a promise
    Promise<Request> read_request();

    Server&      _server;
    ErrorLogger& _elog;

    std::vector<char> _buffer;
    std::string       _request;
    std::string       _response;
};
}  // namespace webserv::server
