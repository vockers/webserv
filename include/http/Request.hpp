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

    void append_body(const std::string& body);

private:
    using MethodMap = std::unordered_map<std::string, Method>;

    Method      _method;
    std::string _uri;
    std::string _query;
    Headers     _headers;
    std::string _body;
    size_t      _content_length;

    void parse_line(const std::string& line);
    void parse_headers(const std::string& headers);

    static const MethodMap METHOD_MAP;
};
}  // namespace webserv::http
