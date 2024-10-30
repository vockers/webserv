#pragma once

#include <sstream>
#include <string>
#include <unordered_map>

#include "config/Config.hpp"
#include "http/Request.hpp"
#include "utils/Logger.hpp"

namespace webserv::http
{
using config::Config;
using utils::ErrorLogger;

class Request;

class Response : public std::stringstream
{
public:
    enum class StatusCode : int
    {
        OK                         = 200,
        CREATED                    = 201,
        MOVED_PERMANENTLY          = 301,
        BAD_REQUEST                = 400,
        FORBIDDEN                  = 403,
        NOT_FOUND                  = 404,
        METHOD_NOT_ALLOWED         = 405,
        REQUEST_ENTITY_TOO_LARGE   = 413,
        INTERNAL_SERVER_ERROR      = 500,
        NOT_IMPLEMENTED            = 501,
        HTTP_VERSION_NOT_SUPPORTED = 505,
    };

    Response(const Request& request, const Config& config, ErrorLogger& elog);
    Response(StatusCode code, const Config& config, ErrorLogger& elog);

    /// @brief Sets the response status code
    ///
    /// @param code Status code
    Response& code(StatusCode code);

    /// @brief Sets the response status code
    ///
    /// @param code Status code
    Response& code(const std::string& code);

    /// @brief Sets a response header
    ///
    /// @param key Header key
    /// @param value Header value
    Response& header(const std::string& key, const std::string& value);

    /// @brief Sets the response body and Content-Length header
    ///
    /// @param body Response body, Content-Length will be set to body.size()
    Response& body(const std::string& body);

    /// @brief Sends a file as the response body
    ///
    /// @param path Path to the file
    Response& file(const std::string& path);

    /// @brief Sets the content type header based on the file extension
    ///
    /// @param extension File extension
    Response& content_type(const std::string& extension);

    /// @brief Write request's multipart/form-data to a file
    ///
    /// @param uri URI of the request
    /// @param body Request body
    Response& upload_file(const std::string& uri, const std::string& body);

    /// @brief Delete a file
    ///
    /// @param uri URI of the request
    Response& delete_file(const std::string& uri);

    /// @brief Generates an autoindex page for a directory with a html template
    ///
    /// @param path Path to the directory
    /// @param uri URI of the request
    Response& autoindex(const std::string& path, const std::string& uri);

    /// @brief Generates an autoindex page for a directory with a buildin template
    ///
    /// @param path Path to the directory
    /// @param uri URI of the request
    Response& autoindex_buildin(const std::string& path, const std::string& uri);

    /// @brief Returns the Content-Length header value
    ///
    /// @return Content-Length header value
    ssize_t get_content_length() const;

    /// @brief Checks if a file exists
    ///
    /// @param path Path to the file
    /// @throw StatusCode::NOT_FOUND if the file does not exist
    static void file_exist(const std::string& path);

    /// @brief Checks if a file is readable
    ///
    /// @param path Path to the file
    /// @throw StatusCode::FORBIDDEN if the file is not readable
    static void file_readable(const std::string& path);

    /// @brief Maps file extension to content type (MIME type)
    ///
    /// @param extension File extension
    /// @return Content type (MIME type)
    static const std::string& get_content_type(const std::string& extension);

    /// @brief Maps status code to its string representation
    ///
    /// @param code Status code
    /// @return String representation of the status code
    static const std::string& code_to_string(StatusCode code);

    static const std::unordered_map<std::string, std::string> CONTENT_TYPES;
    bool                                                      is_cgi(const std::string& uri);

    /// @brief generate a response page from a template
    ///
    /// @param path Path to the template file
    void generate_response_page(const std::string& path);

    /// @brief replace a placeholder in a html template
    ///
    /// @param html The html template
    /// @param placeholder The placeholder to replace
    /// @param value The value to replace the placeholder with
    void replace_placeholder(std::string&       html,
                             const std::string& placeholder,
                             const std::string& value);

private:
    const Config& _config;

    ssize_t _content_length;

    ErrorLogger& _elog;
};
}  // namespace webserv::http
