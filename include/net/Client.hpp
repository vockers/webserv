#pragma once

#include "config/Config.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "net/Socket.hpp"
#include "utils/Logger.hpp"

namespace webserv::net
{
using config::Config;
using http::Request;
using http::Response;
using StatusCode = http::Response::StatusCode;
using utils::ErrorLogger;

class VirtualServer;

class Client : public Socket
{
public:
    Client(Socket&& socket, VirtualServer& server, ErrorLogger& elog);
    ~Client();

    using Socket::get_fd;

    /// Handles the connection by reading a
    /// request and sending a response
    void handle_connection();

    bool is_connected() const;

private:
    /// Asynchronously reads a request from the client
    ///
    /// @return The request as a promise
    Promise<StatusCode> read_request();

    VirtualServer& _server;
    ErrorLogger&   _elog;

    std::unique_ptr<Request>  _request;
    std::unique_ptr<Response> _response;

    std::vector<char> _buffer;
    std::string       _request_str;
    std::string       _response_str;

    bool _is_connected = true;
};
}  // namespace webserv::net
