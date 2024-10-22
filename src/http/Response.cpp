#include "http/Response.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <fstream>
#include <stdexcept>
#include <iostream>

#include "http/Request.hpp"
#include "http/Cgi.hpp"

namespace webserv::http
{
// clang-format off
const std::unordered_map<std::string, std::string> Response::CONTENT_TYPES = {
    {"html",      "text/html"},
    {"css",        "text/css"},
    {"js",  "text/javascript"},
    {"txt",      "text/plain"},

    {"xml", "application/xml"},
    {"x-www-form-urlencoded", "application/x-www-form-urlencoded"},

    {"jpeg",     "image/jpeg"},
    {"jpg",       "image/jpg"},
    {"png",       "image/png"},
    {"gif",       "image/gif"},
    {"ico",    "image/x-icon"},

    {"mpeg",     "audio/mpeg"},
    {"ogg",       "audio/ogg"},

    {"mp4",       "video/mp4"},
    {"webm",     "video/webm"},

    {"form-data", "multipart/form-data"},
};
// clang-format on

Response::Response(const Request& request, const Config& config, ErrorLogger& elog)
    : _content_length(0), _elog(elog)
{
    this->code(StatusCode::OK);
    const Config& location = config.location(request.get_uri());
    std::string   uri      = location.root() + request.get_uri();
    if (uri.ends_with("/")) {
        uri += location.index();
        // TODO: Check autoindex if index file not found
    }

	std::string interpreter;

	if (Cgi::is_cgi_request(uri, interpreter)) {
		Cgi cgi(request, uri, interpreter, elog);
		this->content_type("html");
		this->body(cgi.get_output());
	} 
	else {
   		this->file(uri);
	}
}

Response::Response(StatusCode code, const Config& config, ErrorLogger& elog)
    : _content_length(0), _elog(elog)
{
    try {
        std::string error_page_path = config.error_page(static_cast<int>(code));
        this->code(code);
        const Config& location = config.location(error_page_path);
        this->file(location.root() + error_page_path);
    } catch (...) {
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
        this->content_type("html");
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

    this->content_type(path.substr(path.find_last_of('.') + 1));
    this->body(ss.str());

    return *this;
}

Response& Response::content_type(const std::string& extension)
{
    this->header("Content-Type", get_content_type(extension));

    return *this;
}

ssize_t Response::get_content_length() const
{
    return _content_length;
}

const std::string& Response::get_content_type(const std::string& extension)
{
    static const std::string DEFAULT_CONTENT_TYPE = "text/plain";

    auto it = CONTENT_TYPES.find(extension);
    if (it != CONTENT_TYPES.end()) {
        return it->second;
    } else {
        return DEFAULT_CONTENT_TYPE;
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
}  // namespace webserv::http
