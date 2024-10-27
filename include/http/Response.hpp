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
        BAD_REQUEST                = 400,
        FORBIDDEN                  = 403,
        NOT_FOUND                  = 404,
        INTERNAL_SERVER_ERROR      = 500,
        NOT_IMPLEMENTED            = 501,
        HTTP_VERSION_NOT_SUPPORTED = 505,
    };

    Response(const Request& request, const Config& config, ErrorLogger& elog);
    Response(StatusCode code, const Config& config, ErrorLogger& elog);

    Response& code(StatusCode code);
    Response& code(const std::string& code);
    Response& header(const std::string& key, const std::string& value);
    Response& body(const std::string& body);
    Response& file(const std::string& path);
    Response& content_type(const std::string& extension);
    
	/// @brief Generates an autoindex page for a directory
	///
	/// @param path Path to the directory
    /// @param uri URI of the request
	Response& autoindex(const std::string& path, const std::string& uri);

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

    static const std::unordered_map<std::string, std::string> CONTENT_TYPES;

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

private:
    ssize_t _content_length;

    ErrorLogger& _elog;
};
}  // namespace webserv::http
