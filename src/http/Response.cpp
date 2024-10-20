#include "http/Response.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <fstream>
#include <stdexcept>
#include <unordered_map>

#include "http/Request.hpp"

namespace webserv::http
{
Response::Response(const Request& request, const Config& config, ErrorLogger& elog)
    : _content_length(0), _elog(elog)
{
    this->code(StatusCode::OK);
    const Config& location = config.location(request.get_uri());
    this->file(location.root() + request.get_uri());
}

Response::Response(StatusCode code, const Config& config, ErrorLogger& elog)
    : _content_length(0), _elog(elog)
{
    try {
        std::string error_page_path = config.error_page(static_cast<int>(code));
        this->code(code);
        const Config& location = config.location(error_page_path);
        this->file(location.root() + error_page_path);
    } catch (const std::runtime_error& e) {
        const std::string& code_str = code_to_string(code);
        // clang-format off
        std::string error_page =
        "<!DOCTYPE html>\n"
        "<html lang=\"en-US\"><head><meta charset=\"utf-8\" />\n"
        "    <head>\n"
        "        <title>" + code_str + "</title>\n"
        "    </head>\n"
        "    <body>\n"
        "        <h1 align=\"center\">" + code_str + "</h1>\n"
        "        <p align=\"center\">webserv</p>\n"
        "    </body>\n"
        "</html>\n";
        // clang-format on

        this->code(code_str);
        this->body(error_page);
    }
}

Response& Response::code(StatusCode code)
{
    return this->code(code_to_string(code));
}

Response& Response::code(const std::string& code)
{
    *this << ("HTTP/1.1 ") << code << "\r\n";
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

Response& Response::file(const std::string& path)
{
    std::fstream file(path);
    if (!file.is_open()) {
        throw StatusCode::NOT_FOUND;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    this->body(ss.str());

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
