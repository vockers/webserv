#pragma once

#include <string>
#include <sstream>

namespace webserv::http
{
class Request;

class Response : public std::stringstream
{
public:
    enum class StatusCode
    {
        OK = 200,
        BAD_REQUEST = 400,
        NOT_FOUND = 404,
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        HTTP_VERSION_NOT_SUPPORTED = 505,
    };

    Response() = default;
    Response(const Request& request);

    Response& code(StatusCode code);
    Response& header(const std::string& key, const std::string& value);
    Response& body(const std::string& body);

    static const std::string& code_to_string(StatusCode code);

};
}
