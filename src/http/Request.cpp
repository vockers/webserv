#include "http/Request.hpp"

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
    std::string line = input.substr(0, input.find("\r\n"));

    parse_line(line);
}

void Request::parse_line(const std::string& input)
{
    std::string method = input.substr(0, input.find(' '));

    // Throws 501 (not implemented) if the method is not supported
    auto it = METHOD_MAP.find(method);
    if (it == METHOD_MAP.end()) {
        throw 501;
    }
    _method = it->second;
}
}
