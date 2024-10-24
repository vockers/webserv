#include "net/Client.hpp"

#include <iostream>

#include "http/CGI.hpp"
#include "http/Response.hpp"
#include "net/Server.hpp"
#include "net/VirtualServer.hpp"

namespace webserv::net
{
using http::Response;

Client::Client(Socket&& socket, VirtualServer& server, ErrorLogger& elog)
    : Socket(std::move(socket)), _server(server), _elog(elog)
{
}

Client::~Client()
{
    _elog.log(ErrorLogger::INFO, "Client disconnected from: " + _address.to_string());
}

void Client::handle_connection()
{
    _request.clear();
    _response.clear();

    _elog.log(ErrorLogger::DEBUG, "Handling connection from " + get_address().to_string());
    this->read_request().then([this](std::expected<Request, StatusCode> result) {
        // Create a response and send it back to the client
        try {
            if (!result.has_value()) {
                throw result.error();
            }
            auto&       request   = result.value();
            std::string host_name = request.host().substr(0, request.host().find(':'));
            _response             = Response(request, _server.get_config(host_name), _elog).str();
        } catch (StatusCode status_code) {
            _response = Response(status_code, _server.get_config(""), _elog).str();
        }
        this->write(std::vector<char>(_response.begin(), _response.end()))
            .then([this](ssize_t bytes_written) {
                _elog.log("Sent response to " + get_address().to_string() + ": " +
                          std::to_string(bytes_written) + " bytes");

                // Handle the next request and response
                this->handle_connection();
            });
    });
}

Promise<std::expected<Request, StatusCode>> Client::read_request()
{
    return Promise<std::expected<Request, StatusCode>>(
        [this]() -> std::optional<std::expected<Request, StatusCode>> {
            this->read(_buffer).then([this](ssize_t bytes_read) {
                if (bytes_read == 0) {
                    this->close();
                    return;
                }

                _request += std::string(_buffer.begin(), _buffer.begin() + bytes_read);
                _buffer.clear();

                _elog.log("Received data from " + get_address().to_string() + ": " +
                          std::to_string(bytes_read) + " bytes");
            });

            // Check if the request is complete, i.e. contains two CRLF sequences
            if (_request.find("\r\n\r\n") != std::string::npos) {
                try {
                    return Request(_request);
                } catch (StatusCode status_code) {
                    return std::unexpected(status_code);
                }
            }

            return std::nullopt;
        });
}
}  // namespace webserv::net
