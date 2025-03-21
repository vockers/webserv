#include "net/Client.hpp"

#include <iostream>

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
    _request.reset();
    _response.reset();
    _request_str.clear();
    _response_str.clear();

    this->read_request().then([this](StatusCode status_code) {
        if (_is_connected == false) {
            return;
        }
        _elog.log("Received request from " + get_address().to_string());

        // Create a response and send it back to the client
        std::string host_name = "";
        try {
            if (status_code != StatusCode::OK) {
                throw status_code;
            }
            host_name = _request->host().substr(0, _request->host().find(':'));
            _response.reset(new Response(*_request, _server.get_config(host_name), _elog));
        } catch (StatusCode status_code) {
            _response.reset(new Response(status_code, _server.get_config(host_name), _elog));
        }

        _response->get_output().then([this](const std::string& response_str) {
            this->write(std::vector<char>(response_str.begin(), response_str.end()))
                .then([this](ssize_t bytes_written) {
                    _elog.log("Sent response to " + get_address().to_string() + ": " +
                              std::to_string(bytes_written) + " bytes");

                    // Handle the next request and response
                    this->handle_connection();
                });
        });
    });
}

bool Client::is_connected() const
{
    return _is_connected;
}

Promise<StatusCode> Client::read_request()
{
    return Promise<StatusCode>([this]() -> std::optional<StatusCode> {
        if (this->_fd == -1) {
            this->_is_connected = false;
            return StatusCode::OK;
        }

        // Read data from the client's socket and append it to the request
        this->read(_buffer).then([this](ssize_t bytes_read) {
            if (bytes_read == 0) {
                this->close();
                return;
            }

            if (_request) {
                _request->append_body(std::string(_buffer.begin(), _buffer.begin() + bytes_read));
            } else {
                _request_str += std::string(_buffer.begin(), _buffer.begin() + bytes_read);
            }
            _buffer.clear();

            _elog.log("Received data from " + get_address().to_string() + ": " +
                      std::to_string(bytes_read) + " bytes");
        });

        // Check if the request body is complete
        if (_request) {
            if (_request->chunked() &&
                _request->body().find("\r\n0\r\n\r\n") != std::string::npos) {
                _request->unchunk_body();
                return StatusCode::OK;
            } else {
                if (_request->content_length() == _request->body().size()) {
                    return StatusCode::OK;
                } else if (_request->content_length() < _request->body().size()) {
                    return StatusCode::BAD_REQUEST;
                }
            }
            return std::nullopt;
        }

        // Check if the request-line and headers are complete
        if (_request_str.find("\r\n\r\n") != std::string::npos) {
            try {
                _request.reset(new Request(_request_str));
                if (_request->chunked() || _request->content_length() > 0) {
                    return std::nullopt;
                }
                return StatusCode::OK;
            } catch (StatusCode status_code) {
                return status_code;
            }
        }

        return std::nullopt;
    });
}
}  // namespace webserv::net
