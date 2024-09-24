#include "http/Request.hpp"

#include <sstream>

namespace webserv::http
{
// clang-format off
const Request::MethodMap Request::METHOD_MAP = {
    {"GET",    Method::GET},
    {"POST",   Method::POST},
    {"DELETE", Method::DELETE},
};
// clang-format on

Request::Request(const std::string& input)
{
    size_t headers_start = input.find("\r\n");
    size_t headers_end = input.find("\r\n\r\n");
    if (headers_start == std::string::npos || headers_end == std::string::npos) {
        throw 400;
    }

    std::string line = input.substr(0, headers_start);
    parse_line(line);

    std::string headers = input.substr(headers_start + 2, headers_end - headers_start);
    parse_headers(headers);
}

Request::Method Request::get_method() const
{
    return _method;
}

const Request::Headers& Request::get_headers() const
{
    return _headers;
}

const std::string& Request::get_uri() const
{
    return _uri;
}

void Request::parse_line(const std::string& line)
{
    std::stringstream line_stream(line);

    std::string method, version;
    line_stream >> method >> _uri >> version;

    // Throws 501 (not implemented) if the method is not supported
    auto it = METHOD_MAP.find(method);
    if (it == METHOD_MAP.end()) {
        throw 501;
    }
    _method = it->second;

    if (version != "HTTP/1.1") {
        throw 505;
    }
}

void Request::parse_headers(const std::string& headers)
{
    std::stringstream headers_stream(headers);

    std::string header;
    while (std::getline(headers_stream, header, '\n')) {
        if (header == "\r") {
            break;
        }

        size_t colon_pos = header.find(':');
        if (colon_pos == std::string::npos) {
            throw 400;
        }

        std::string key = header.substr(0, colon_pos);
        std::string value = header.substr(colon_pos + 2, header.size() - colon_pos - 3);

        _headers[key] = value;
    }
}
}
