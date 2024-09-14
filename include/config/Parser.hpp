#pragma once

#include <map>

#include "config/Directive.hpp"
#include "config/Lexer.hpp"

namespace webserv::config
{
class Parser
{
public:
    using Token = Lexer::Token;

    Parser(const std::string& input);

    Directive parse();

private:
    using AllowedDirectivesMap = std::map<std::string, std::vector<std::string>>;

    Token _next_token;
    Lexer _lexer;

    /// Throws if the next token is not of type `type`
    ///
    /// @param type The expected token type
    ///
    /// @return The token
    Token take_token(Token::Type type);

    /// Throws if a directive is allowed in a parent directive
    ///
    /// @param parent_directive The parent directive
    /// @param directive The directive
    static void is_directive_allowed(const std::string& parent_directive,
                                     const std::string& directive);

    /// Recursively parse a directive
    ///
    /// @param parent_directive The parent directive
    ///
    /// @return The parsed directive
    Directive parse_directive(const std::string& parent_directive);

    /// Allowed directives for each parent directive
    ///
    /// ("") is the main directive
    // clang-format off
    static inline const AllowedDirectivesMap _allowed_directives = {
        {"",                     {"http", "log_level"}},
        {"http",                 {"autoindex", "client_max_body_size",
                                  "error_page", "index", "root", "server", "upload_dir"}},
        {"location",             {"autoindex", "client_max_body_size",
                                  "error_page", "index", "limit_except",
                                  "location", "return", "root", "upload_dir"}},
        {"server",               {"autoindex", "client_max_body_size",
                                  "error_page", "index", "listen", "location",
                                  "return", "root", "server_name", "upload_dir"}},
    };
    // clang-format on
};
}  // namespace webserv::config
