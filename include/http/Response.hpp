#pragma once

#include <string>

#include "http/Request.hpp"
#include "server/Pollable.hpp"
#include "utils/Logger.hpp"

namespace webserv::http 
{
using server::Readable;
using server::Writable;
using utils::ErrorLogger;

class Request;

class Response : public Readable, public Writable
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

    Response(const Request& request, ErrorLogger& elog);

    Response& code(StatusCode code);
    Response& header(const std::string& key, const std::string& value);
    Response& body(const std::string& body);

    void handle_read() override;
    void handle_write() override;

    static const std::string& code_to_string(StatusCode code);

private:
    ssize_t _content_length;

    ErrorLogger&   _elog;
    const Request& _request;
};
}
