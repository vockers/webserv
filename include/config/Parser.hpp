#pragma once

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
    Token _next_token;
    Lexer _lexer;

    /// Throws if the next token is not of type `type`
    ///
    /// @param type The expected token type
    ///
    /// @return The token
    Token take_token(Token::Type type);

    void parse_parameters(Directive& directive);

    /// Recursively parse a directive
    ///
    /// @param parent_directive The parent directive
    ///
    /// @return The parsed directive
    Directive parse_directive(const Directive::Directives& siblings, Directive* parent);
};
}  // namespace webserv::config
