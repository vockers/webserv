#include "config/Parser.hpp"

#include <stdexcept>

namespace webserv::config
{
Parser::Parser(const std::string& input)
    : _lexer(input)
{
    _next_token = _lexer.next_token();
}

Directive Parser::parse()
{
    if (_next_token.type == Token::Type::NONE) {
        return Directive("", {}, {});
    }

    return parse_directive();
}

Parser::Token Parser::take_token(Token::Type type)
{
    Token token = _next_token;
    
    if (token.type == Token::Type::NONE) {
        std::runtime_error("Unexpected end of input");
    }
    if (token.type != type) {
        throw std::runtime_error("Unexpected token: " + Token::type_to_string(token.type) + " expected: " + Token::type_to_string(type));
    }

    _next_token = _lexer.next_token();
    return token;
}

Directive Parser::parse_directive()
{
    std::string           name;
    Directive::Parameters parameters;
    Directive::Directives children;

    if (_next_token.type == Token::Type::WORD) {
        name = *_next_token.value;
        _next_token = _lexer.next_token();
    } else {
        throw std::runtime_error("Expected 'word' token");
    }
    while (_next_token.type == Token::Type::WORD) {
        parameters.push_back(*_next_token.value);
        _next_token = _lexer.next_token();
    }

    if (_next_token.type == Token::Type::BLOCK_START) {
        _next_token = _lexer.next_token();

        while (_next_token.type != Token::Type::BLOCK_END) {
            children.push_back(parse_directive());
        }
    } else {
        if (parameters.size() < 1) {
            throw std::runtime_error("Expected at least one parameter");
        }
        if (_next_token.type != Token::Type::END) {
            throw std::runtime_error("Expected '{' or ';'");
        }
    }
    
    _next_token = _lexer.next_token();

    return Directive(name, parameters, children);
}
}  // namespace webserv::config
