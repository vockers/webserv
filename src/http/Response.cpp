#include "http/Response.hpp"

#include <fstream>
#include <unordered_map>

#include "http/Request.hpp"

namespace webserv::http
{
Response::Response(const Request& request)
{
    code(StatusCode::OK);

    std::fstream file("www/default" + request.get_uri());
    std::stringstream buffer;
    buffer << file.rdbuf();
    body(buffer.str());
}

Response& Response::code(StatusCode code)
{
    *this << "HTTP/1.1 " << code_to_string(code) << "\r\n";
    return *this;
}

Response& Response::header(const std::string& key, const std::string& value)
{
    *this << key << ": " << value << "\r\n";
    return *this;
}

Response& Response::body(const std::string& body)
{
    *this << "Content-Length: " << body.size() << "\r\n\r\n";
    *this << body;
    return *this;
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
