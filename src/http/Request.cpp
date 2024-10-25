#include "http/Request.hpp"

#include <sstream>

#include "http/Response.hpp"

namespace webserv::http
{
using StatusCode = Response::StatusCode;

// clang-format off
const Request::MethodMap Request::METHOD_MAP = {
    {"GET",    Method::GET},
    {"POST",   Method::POST},
    {"DELETE", Method::DELETE},
};
// clang-format on

Request::Request(const std::string& input) : _content_length(0)
{
    size_t headers_start = input.find("\r\n");
    size_t headers_end   = input.find("\r\n\r\n");
    if (headers_start == std::string::npos || headers_end == std::string::npos) {
        throw StatusCode::BAD_REQUEST;
    }

    std::string line = input.substr(0, headers_start);
    parse_line(line);

    std::string headers = input.substr(headers_start + 2, headers_end - headers_start);
    parse_headers(headers);

    // Add remaining data as the body
    if (_content_length > 0 && input.size() > headers_end + 4) {
        _body = input.substr(headers_end + 4);
    }
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

const std::string& Request::host() const
{
    return _headers.at("host");
}

const std::string& Request::body() const
{
    return _body;
}

size_t Request::content_length() const
{
    return _content_length;
}

void Request::append_body(const std::string& body)
{
    _body += body;
}

void Request::parse_line(const std::string& line)
{
    std::stringstream line_stream(line);

    std::string method, version;
    line_stream >> method >> _uri >> version;
    if (line_stream.fail()) {
        throw StatusCode::BAD_REQUEST;
    }

    // Throws 501 (not implemented) if the method is not supported
    auto it = METHOD_MAP.find(method);
    if (it == METHOD_MAP.end()) {
        throw StatusCode::NOT_IMPLEMENTED;
    }
    _method = it->second;

    if (version != "HTTP/1.1") {
        throw StatusCode::HTTP_VERSION_NOT_SUPPORTED;
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
            throw StatusCode::BAD_REQUEST;
        }

        std::string key   = header.substr(0, colon_pos);
        std::string value = header.substr(colon_pos + 2, header.size() - colon_pos - 3);
        // field-name (key) is case-insensitive
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);

        _headers[key] = value;
    }

    // Host header is mandatory
    if (_headers.find("host") == _headers.end()) {
        throw StatusCode::BAD_REQUEST;
    }

    try {
        _content_length = std::stoul(_headers.at("content-length"));
    } catch (const std::exception& e) {
        _content_length = 0;
    }
}
}  // namespace webserv::http
