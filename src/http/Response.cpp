#include "http/Response.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <unordered_map>

#include "http/Request.hpp"

namespace webserv::http
{
Response::Response(const Request& request, ErrorLogger& elog)
    : _content_length(0), _elog(elog)
{
    code(StatusCode::OK);
    elog.log("Request URI: " + request.get_uri());

    int fd = open(std::string("www/default" + request.get_uri()).c_str(), O_RDONLY);
    if (fd == -1) {
        throw StatusCode::NOT_FOUND;
    }
    /*Readable::set_fd(fd);*/
    /*Readable::set_state(FDStatus::POLLING);*/
}

Response& Response::code(StatusCode code)
{
    /*_writer.buffer() << ("HTTP/1.1 ") << code_to_string(code) << "\r\n";*/
    return *this;
}

Response& Response::header(const std::string& key, const std::string& value)
{
    /*_writer.buffer() << (key.c_str()) << ": " << value << "\r\n";*/
    return *this;
}

Response& Response::body(const std::string& body)
{
    _content_length = body.size();
    /*_writer.buffer() << ("Content-Length: ") << _content_length << "\r\n\r\n" << body;*/
    return *this;
}

ssize_t Response::get_content_length() const
{
    return _content_length;
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
}  // namespace webserv::http
