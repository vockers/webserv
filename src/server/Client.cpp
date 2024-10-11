#include "server/Client.hpp"

#include "http/Response.hpp"
#include "server/Server.hpp"

namespace webserv::server
{
/*using StatusCode = Response::StatusCode;*/
using http::Response;

Client::Client(Socket&& socket, Server& server, ErrorLogger& elog)
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

    this->read_request().then([this](Request request) {
        _elog.log(ErrorLogger::DEBUG, "Received request from " + get_address().to_string());

        // Create a response and send it back to the client
        _response = Response(request, _elog).str();
        this->write(std::vector<char>(_response.begin(), _response.end()))
            .then([this](ssize_t bytes_written) {
                _elog.log(ErrorLogger::DEBUG,
                          "Sent response to " + get_address().to_string() + ": " +
                              std::to_string(bytes_written) + " bytes");

                // Handle the next request and response
                this->handle_connection();
            });
    });
}

Promise<Request> Client::read_request()
{
    return Promise<Request>([this]() -> std::optional<Request> {
        this->read(_buffer).then([this](ssize_t bytes_read) {
            _request += std::string(_buffer.begin(), _buffer.begin() + bytes_read);
            _buffer.clear();

            _elog.log(ErrorLogger::DEBUG,
                      "Received data from " + get_address().to_string() + ": " +
                          std::to_string(bytes_read) + " bytes");
        });

        // Check if the request is complete, i.e. contains two CRLF sequences
        if (_request.find("\r\n\r\n") != std::string::npos) {
            return Request(_request);
        }

        return std::nullopt;
    });
}
}  // namespace webserv::server
