#pragma once

#include <string>
#include <unordered_map>

namespace webserv::http
{
class Request
{
public:
    using Headers = std::unordered_map<std::string, std::string>;

    enum class Method
    {
        GET,
        POST,
        DELETE,
    };

    Request(const std::string& input);

    Method             get_method() const;
    const std::string& get_uri() const;
    const std::string& get_query() const;
    const Headers&     get_headers() const;
    const std::string& host() const;
    const std::string& body() const;
    size_t             content_length() const;
    bool               chunked() const;

    /// @brief Append to the body of the request.
    ///
    /// @param body The body to append.
    void append_body(const std::string& body);

    /// @brief Remove the chunked encoding from the body.
    void unchunk_body();

private:
    using MethodMap = std::unordered_map<std::string, Method>;

    Method      _method;
    std::string _uri;
    std::string _query;
    Headers     _headers;
    std::string _body;
    size_t      _content_length;
    bool        _chunked;

    void parse_line(const std::string& line);
    void parse_headers(const std::string& headers);

    static const MethodMap METHOD_MAP;
    static constexpr int   HEADER_LIMIT = 8192;
};
}  // namespace webserv::http
