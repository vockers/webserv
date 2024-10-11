#include "server/Client.hpp"

#include <algorithm>
#include <iostream>

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

    this->read_request().then([this](Request request) {
        /*_request = request;*/
        _elog.log(ErrorLogger::DEBUG, "Received request from " + get_address().to_string());
        /*_response = Response(_request, *this, _elog);*/
        /*_response.poll().then([this](Poll poll) {*/
        /*    if (poll == Poll::READY) {*/
        /*        _server.add_blocking_event(*/
        /*            EventHandler(_response.get_fd(), std::bind(&Response::poll, &_response)));*/
        /*    }*/
        /*});*/
        _response = Response(request, _elog).str();
        this->write(std::vector<char>(_response.begin(), _response.end()))
            .then([this](ssize_t bytes_written) {
                _elog.log(ErrorLogger::DEBUG,
                          "Sent response to " + get_address().to_string() + ": " +
                              std::to_string(bytes_written) + " bytes");
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

        if (_request.find("\r\n\r\n") != std::string::npos) {
            return Request(_request);
        }

        return std::nullopt;
    });
}

/*void Client::handle_read()*/
/*{*/
/*    while (Readable::get_state() == FDStatus::POLLING)*/
/*    {*/
/*        ssize_t bytes_read = Readable::read();*/
/*        if (bytes_read == -1) {*/
/*            if (errno == EAGAIN || errno == EWOULDBLOCK) {*/
/*                _elog.log("No more data available to read right now");*/
/*                // No more data available to read right now*/
/*                return;*/
/*            }*/
/*            // Handle other read errors*/
/*            _elog.log(ErrorLogger::ERROR, "Error reading from socket");*/
/*            return;*/
/*        }*/
/**/
/*        else if (bytes_read == 0) {*/
/*            // TODO: Handle client disconnection*/
/*            // Connection closed by the client*/
/*            _elog.log(ErrorLogger::INFO, "Client disconnected from " +
 * get_address().to_string());*/
/*            return;*/
/*        }*/
/**/
/*        _elog.log("Bytes received from " + get_address().to_string() + ": " +*/
/*                  std::to_string(bytes_read));*/
/**/
/*        if (Readable::buffer().str().find("\r\n\r\n") != std::string::npos) {*/
/*            Readable::set_state(FDStatus::DONE);*/
/*        }*/
/*    }*/
/**/
/*    try {*/
/*        _request.reset(new Request(Readable::buffer().str()));*/
/*        Readable::buffer().str("");  // Clear the buffer*/
/*        _response.reset(new Response(*_request, *this, _elog));*/
/*        EventHandler event(_response->get_fd(),*/
/*                           std::bind(&Response::Readable::poll, _response.get()));*/
/**/
/*        _server.add_blocking_event(event);*/
/*    } catch (StatusCode code) {*/
/*        _elog.log(ErrorLogger::ERROR,*/
/*                  "Error parsing request: " + Response::code_to_string(code));*/
/*        return;*/
/*    }*/
/*}*/

/*void Client::handle_write()*/
/*{*/
/*    ssize_t bytes_written = Writable::write();*/
/*    if (bytes_written == -1) {*/
/*        if (errno == EAGAIN || errno == EWOULDBLOCK) {*/
/*            _elog.log("Socket buffer full, can't write more data right now");*/
/*            return;  // Exit the loop to return control to the event loop*/
/*        }*/
/*        // Handle other write errors*/
/*        _elog.log(ErrorLogger::ERROR, "Error writing to socket");*/
/*        return;*/
/*    }*/
/*    _elog.log("Bytes written to " + get_address().to_string() + ": " +*/
/*              std::to_string(bytes_written));*/
/**/
/*    if (Writable::get_bytes() >= (ssize_t)Writable::buffer().str().size()) {*/
/*        Writable::set_state(FDStatus::DONE);*/
/*    }*/
/*}*/
}  // namespace webserv::server
