#include "http/Response.hpp"

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

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
    : _config(config), _content_length(0), _elog(elog), _cgi(nullptr)
{
    const Config& location = config.location(request.get_uri());
    std::string   path     = location.root() + request.get_uri();

    if (request.body().size() > location.client_max_body_size()) {
        throw StatusCode::REQUEST_ENTITY_TOO_LARGE;
    }
    if (location.limit_except(request.method_str()) == false) {
        throw StatusCode::METHOD_NOT_ALLOWED;
    }

    // Redirect if return directive is set
    if (location.return_uri() != "") {
        this->code(static_cast<StatusCode>(location.return_code()));
        this->header("Location", location.return_uri());
        this->body("");
        return;
    }

    std::string interpreter;
    if (CGI::is_cgi_request(path, interpreter)) {
        try {
            this->code(StatusCode::OK);
            _cgi.reset(new CGI(request, path, interpreter));
            return;
        } catch (StatusCode status_code) {
            throw status_code;
        }
    }
    switch (request.get_method()) {
    case Request::Method::GET:
        if (path.ends_with("/")) {
            try {
                this->code(StatusCode::OK);
                this->file(path + location.index());
            } catch (StatusCode status_code) {
                if (location.autoindex()) {
                    // this->code(StatusCode::OK);
                    this->autoindex(path, request.get_uri());
                } else {
                    throw status_code;
                }
            }
            break;
        }
        this->code(StatusCode::OK);
        this->file(path);
        break;
    case Request::Method::POST:
        this->upload_file(request.get_uri(), request.body());
        break;
    case Request::Method::DELETE:
        this->delete_file(request.get_uri());
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

        std::ifstream template_file("www/default/error.html");
        if (!template_file.is_open()) {
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
            return;
        }
        std::stringstream buffer;
        buffer << template_file.rdbuf();

        template_file.close();

        std::string html = buffer.str();
        size_t      pos  = html.find("{{STATUS_CODE}}");
        html.replace(pos, 15, code_str);

        this->code(code_str);
        this->content_type("html");
        this->body(html);
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
        // This already checks in the file_exists and is_readable right?
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

bool Response::is_cgi(const std::string& uri)
{
    // same as in CGI::is_cgi_request but without the interpreter...
    std::string ext      = ".py";
    int         ext_size = ext.size();

    if (uri.size() >= ext.size() && uri.compare(uri.size() - ext_size, ext_size, ext) == 0) {
        return true;
    }
    return false;
}

void Response::generate_response_page(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        throw StatusCode::INTERNAL_SERVER_ERROR;
    }

    std::string html_content((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
    file.close();

    this->content_type("html");
    this->body(html_content);
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
        throw StatusCode::FORBIDDEN;
    }

    int permissions = is_cgi(path) ? 0755 : 0644;

    int fd = open(path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, permissions);
    if (fd == -1) {
        throw StatusCode::INTERNAL_SERVER_ERROR;
    }

    if (::write(fd, data.c_str(), data.size()) == -1) {
        close(fd);
        throw StatusCode::INTERNAL_SERVER_ERROR;
    }
    close(fd);
    this->code(StatusCode::CREATED);
    try {
        generate_response_page("www/default/upload_success.html");
    } catch (...) {
        this->content_type("html");
        this->body("File uploaded successfully");
    }
    // TODO: after creation, status code should be 201
    return *this;
}

Response& Response::delete_file(const std::string& uri)
{
    const Config& location = _config.location(uri);
    std::string   path     = location.root() + uri;

    file_exist(path);
    file_readable(path);

    if (path == location.root() + location.upload_dir()) {
        throw StatusCode::FORBIDDEN;
    }

    if (std::filesystem::remove(path) == false) {
        throw StatusCode::INTERNAL_SERVER_ERROR;
    }

    this->code(StatusCode::OK);

    try {
        generate_response_page("www/default/deletion_success.html");
    } catch (...) {
        this->content_type("html");
        this->body("File deleted successfully");
    }
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
		{ StatusCode::CREATED, "201 Created" },
        { StatusCode::MOVED_PERMANENTLY, "301 Moved Permanently" },
        { StatusCode::BAD_REQUEST, "400 Bad Request" },
		{ StatusCode::FORBIDDEN, "403 Forbidden" },
        { StatusCode::NOT_FOUND, "404 Not Found" },
        { StatusCode::METHOD_NOT_ALLOWED, "405 Method Not Allowed" },
        { StatusCode::REQUEST_ENTITY_TOO_LARGE, "413 Request Entity Too Large" },
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

void Response::replace_placeholder(std::string&       html,
                                   const std::string& placeholder,
                                   const std::string& value)
{
    size_t pos = 0;
    while ((pos = html.find(placeholder, pos)) != std::string::npos) {
        html.replace(pos, placeholder.length(), value);
        pos += value.length();
    }
}

Promise<std::string> Response::get_output()
{
    return Promise<std::string>([this]() -> std::optional<std::string> {
        if (_cgi && _cgi->state() != CGI::State::DONE) {
            if (_cgi->state() == CGI::State::IDLE) {
                _cgi->get_output().then([this](const std::string& output) {
                    this->content_type("html");
                    this->body(output);
                });
            }
            if (_cgi->state() != CGI::State::DONE) {
                return std::nullopt;
            }
        }
        return this->str();
    });
}

Response& Response::autoindex_buildin(const std::string& path, const std::string& uri)
{
    std::string html =
        "<!DOCTYPE html>\n"
        "<html lang=\"en-US\"><head><meta charset=\"utf-8\" />\n"
        "    <head>\n"
        "		<title>Index of " +
        uri +
        "</title>\n"
        "    </head>\n"
        "    <body>\n"
        "		<h1>Index of " +
        uri +
        "</h1>\n"
        "    <ul>\n";

    DIR* dir = opendir(path.c_str());
    if (!dir) {
        throw StatusCode::FORBIDDEN;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == ".." || name == ".gitignore")
            continue;

        std::string entry_path = uri + name;
        struct stat file_stat;
        if (stat((path + "/" + name).c_str(), &file_stat) == 0) {
            html += "<li><a href=\"" + entry_path + (S_ISDIR(file_stat.st_mode) ? "/" : "") +
                    "\">" + name + "</a></li>\n";
        }
    }
    closedir(dir);

    html +=
        "        </ul>\n"
        "    </body>\n"
        "</html>\n";

    this->content_type("html");
    this->body(html);

    return *this;
}

Response& Response::autoindex(const std::string& path, const std::string& uri)
{
    // generate_response_page("www/default/autoindex.html"); (body access?)
    std::ifstream template_file("www/default/autoindex.html");
    if (!template_file.is_open()) {
        return (autoindex_buildin(path, uri));
    }

    std::stringstream buffer;
    buffer << template_file.rdbuf();
    template_file.close();
    std::string html = buffer.str();

    replace_placeholder(html, "{{URI}}", uri);

    DIR* dir = opendir(path.c_str());
    if (!dir) {
        throw StatusCode::FORBIDDEN;
    }

    std::string    entries;
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == ".." || name == ".gitignore")
            continue;

        std::string entry_path = uri + name;
        struct stat file_stat;
        if (stat((path + "/" + name).c_str(), &file_stat) == 0) {
            entries += "<li><a href=\"" + entry_path + (S_ISDIR(file_stat.st_mode) ? "/" : "") +
                       "\">" + name + "</a></li>\n";
        }
    }
    closedir(dir);

    replace_placeholder(html, "{{DIRECTORY_ENTRIES}}", entries);

    this->content_type("html");
    this->body(html);

    return *this;
}

}  // namespace webserv::http
