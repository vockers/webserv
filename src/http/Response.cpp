#include "http/Response.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <unordered_map>

#include "http/Request.hpp"

namespace webserv::http
{
using server::FDStatus;

Response::Response(const Request& request, ErrorLogger& elog)
    : _content_length(0), _elog(elog), _request(request)
{
    code(StatusCode::OK);

    int fd = open(std::string("www/default" + request.get_uri()).c_str(), O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        throw StatusCode::NOT_FOUND;
    }
    Readable::set_fd(fd);
    Readable::set_state(FDStatus::POLLING);
    Writable::set_fd(request.get_fd());
}

Response& Response::code(StatusCode code)
{
    Writable::operator<<("HTTP/1.1 ") << code_to_string(code) << "\r\n";
    return *this;
}

Response& Response::header(const std::string& key, const std::string& value)
{
    Writable::operator<<(key.c_str()) << ": " << value << "\r\n";
    return *this;
}

Response& Response::body(const std::string& body)
{
    Writable::operator<<("Content-Length: ") << body.size() << "\r\n\r\n" << body;
    _content_length = body.size();
    return *this;
}

void Response::handle_read()
{
    ssize_t bytes_read = Readable::read();
    if (bytes_read == -1) {
        _elog.log(ErrorLogger::ERROR, "Error reading from file");
        return;
    }
    if (bytes_read == 0) {
        Readable::set_state(FDStatus::DONE);
        body(Readable::str());
        Writable::set_state(FDStatus::POLLING);
        close(Readable::get_fd());
    }
}

void Response::handle_write()
{
    ssize_t bytes_written = Writable::write();
    if (bytes_written == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            _elog.log("Socket buffer full, can't write more data right now");
            return;  // Exit the loop to return control to the event loop
        }
        // Handle other write errors
        _elog.log(ErrorLogger::ERROR, "Error writing to socket");
        return;
    }
    _elog.log("Bytes written to " + _request.get_client().get_address().to_string() + ": " +
              std::to_string(bytes_written));

    if (Writable::get_bytes() == _content_length) {
        Writable::set_state(FDStatus::DONE);
    }
}

const std::string& Response::code_to_string(StatusCode code)
{
    // clang-format off
    static const std::unordered_map<StatusCode, std::string>  STATUS_CODES = {
        { StatusCode::OK, "200 OK" },
        { StatusCode::BAD_REQUEST, "400 Bad Request" },
        { StatusCode::NOT_FOUND, "404 Not Found" },
        { StatusCode::INTERNAL_SERVER_ERROR, "500 Internal Server Error" },
        { StatusCode::NOT_IMPLEMENTED, "501 Not Implemented" },
        { StatusCode::HTTP_VERSION_NOT_SUPPORTED, "505 HTTP Version Not Supported" },
    };
    // clang-format on

    return STATUS_CODES.at(code);
}
}
