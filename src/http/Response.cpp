#include "http/Response.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <fstream>
#include <unordered_map>

#include "http/Request.hpp"

namespace webserv::http
{
Response::Response(const Request& request, ErrorLogger& elog) : _content_length(0), _elog(elog)
{
    this->code(StatusCode::OK);
    elog.log("Request URI: " + request.get_uri());

    std::fstream file(std::string("www/default" + request.get_uri()),
                      std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw StatusCode::NOT_FOUND;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    this->body(ss.str());
}

Response::Response(StatusCode code, ErrorLogger& elog) : _content_length(0), _elog(elog)
{
    const std::string& code_str = code_to_string(code);
    // clang-format off
    std::string error_page =
    "<!DOCTYPE html>"
    "<html lang=\"en-US\"><head><meta charset=\"utf-8\" />"
        "<head>"
            "<title>" + code_str + "</title>"
        "</head>"
        "<body>"
            "<h1 align=\"center\">" + code_str + "</h1>"
            "<p align=\"center\">webserv</p>"
        "</body>"
    "</html>";
    // clang-format on

    this->code(code);
    this->body(error_page);
}

Response& Response::code(StatusCode code)
{
    *this << ("HTTP/1.1 ") << code_to_string(code) << "\r\n";
    return *this;
}

Response& Response::header(const std::string& key, const std::string& value)
{
    *this << (key.c_str()) << ": " << value << "\r\n";
    return *this;
}

Response& Response::body(const std::string& body)
{
    _content_length = body.size();
    *this << ("Content-Length: ") << _content_length << "\r\n\r\n" << body;
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
