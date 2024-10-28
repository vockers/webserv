#include "http/Response.hpp"

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

#include "http/CGI.hpp"
#include "http/Request.hpp"

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
    : _config(config), _content_length(0), _elog(elog)
{
    this->code(StatusCode::OK);
    const Config& location = config.location(request.get_uri());
    std::string   path     = location.root() + request.get_uri();

    std::string interpreter;
    if (CGI::is_cgi_request(path, interpreter)) {
        try {
            CGI cgi(request, path, interpreter);
            this->content_type("html");
            this->body(cgi.get_output());
            return;
        } catch (StatusCode status_code) {
            throw status_code;
        }
    }

    switch (request.get_method()) {
    case Request::Method::GET:
    case Request::Method::DELETE:
        if (path.ends_with("/")) {
            try {
                this->file(path + location.index());
            } catch (StatusCode status_code) {
                if (location.autoindex()) {
                    this->autoindex(path, request.get_uri());
                } else {
                    throw status_code;
                }
            }
            break;
        }
        this->file(path);
        break;
    case Request::Method::POST:
        this->upload_file(request.get_uri(), request.body());
        break;
    default:
        throw StatusCode::NOT_IMPLEMENTED;
    }
}

Response::Response(StatusCode code, const Config& config, ErrorLogger& elog)
    : _config(config), _content_length(0), _elog(elog)
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
    this->file_exist(path);
    this->file_readable(path);

    std::fstream file(path);
    if (!file.is_open()) {
        throw StatusCode::NOT_FOUND;
        // TODO: Throw Forbidden if no read permission
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

Response& Response::upload_file(const std::string& uri, const std::string& body)
{
    std::string boundary = body.substr(0, body.find("\r\n"));
    size_t      pos      = body.find("filename=\"") + 10;
    std::string filename = body.substr(pos, body.find("\"", pos) - pos);
    std::string data     = body.substr(body.find("\r\n\r\n") + 4);
    data                 = data.substr(0, data.find(boundary) - 2);

    const Config& location = _config.location(uri);

    // Check if their is an upload directory
    std::string path = "";
    try {
        path = location.upload_dir() + filename;
    } catch (...) {
        throw StatusCode::INTERNAL_SERVER_ERROR;
        // TODO: Throw Forbidden if no upload directory
    }

    std::ofstream file(path);
    if (!file.is_open()) {
        throw StatusCode::INTERNAL_SERVER_ERROR;
        // TODO: Throw Forbidden if no write permission
    }

    file << data;

    this->content_type("txt");
    this->body("File uploaded");

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
		{ StatusCode::FORBIDDEN, "403 Forbidden" },
        { StatusCode::NOT_FOUND, "404 Not Found" },
        { StatusCode::INTERNAL_SERVER_ERROR, "500 Internal Server Error" },
        { StatusCode::NOT_IMPLEMENTED, "501 Not Implemented" },
        { StatusCode::HTTP_VERSION_NOT_SUPPORTED, "505 HTTP Version Not Supported" },
    };
    // clang-format on

    return STATUS_CODES.at(code);
}

void Response::file_exist(const std::string& path)
{
    if (access(path.c_str(), F_OK) == -1) {
        throw StatusCode::NOT_FOUND;
    }
}

void Response::file_readable(const std::string& path)
{
    if (access(path.c_str(), R_OK) == -1) {
        throw StatusCode::FORBIDDEN;
    }
}

Response& Response::autoindex(const std::string& path, const std::string& uri)
{
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        throw StatusCode::FORBIDDEN;
    }

    // clang-format off
	std::string html = "<!DOCTYPE html>\n"
                       "<html lang=\"en-US\"><head><meta charset=\"utf-8\" />\n"
                       "    <head>\n"
                       "        <title>Index of " + uri + "</title>\n"
                       "    </head>\n"
                       "    <body>\n"
                       "        <h1>Index of " + uri + "</h1>\n"
                       "        <ul>\n";
    // clang-format on

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;

        if (name == "." || name == "..")
            continue;

        std::string full_path = path + "/" + name;

        struct stat file_stat;
        if (stat(full_path.c_str(), &file_stat) == 0) {
            html += "            <li><a href=\"" + name;
            if (S_ISDIR(file_stat.st_mode)) {
                html += "/";
            }
            html += "\">" + name + "</a></li>\n";
        }
    }

    closedir(dir);

    // clang-format off
    html += "        </ul>\n"
            "        <hr/><p style='text-align: center;'>webserv</p>\n"
            "    </body>\n"
            "</html>\n";
    // clang-format on

    this->content_type("html");
    this->body(html);

    return *this;
}

}  // namespace webserv::http
