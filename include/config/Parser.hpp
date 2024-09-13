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

    Token     take_token(Token::Type type);
    Directive parse_directive();
};
}  // namespace webserv::config
