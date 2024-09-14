#include "config/Parser.hpp"

#include <algorithm>
#include <stdexcept>

namespace webserv::config
{
Parser::Parser(const std::string& input) : _lexer(input)
{
    _next_token = _lexer.next_token();
}

Directive Parser::parse()
{
    Directive::Directives children;

    while (_next_token.type != Token::Type::NONE) {
        children.push_back(this->parse_directive(""));
    }

    return Directive("", {}, children);
}

Parser::Token Parser::take_token(Token::Type type)
{
    Token token = _next_token;

    if (token.type == Token::Type::NONE) {
        std::runtime_error("Unexpected end of input");
    }
    if (token.type != type) {
        throw std::runtime_error("Unexpected token: " + Token::type_to_string(token.type) +
                                 " expected: " + Token::type_to_string(type));
    }

    _next_token = _lexer.next_token();
    return token;
}

void Parser::is_directive_allowed(const std::string& parent_directive, const std::string& directive)
{
    auto it = _allowed_directives.find(parent_directive);
    if (it == _allowed_directives.end()) {
        throw std::runtime_error("Directive '" + directive + "' is not allowed in '" +
                                 parent_directive + "'");
    }
    if (std::find(it->second.begin(), it->second.end(), directive) == it->second.end()) {
        throw std::runtime_error("Directive '" + directive + "' is not allowed in '" +
                                 parent_directive + "'");
    }
}

Directive Parser::parse_directive(const std::string& parent_directive)
{
    std::string           name;
    Directive::Parameters parameters;
    Directive::Directives children;

    if (_next_token.type == Token::Type::WORD) {
        name = *_next_token.value;
        this->is_directive_allowed(parent_directive, name);
        this->take_token(Token::Type::WORD);
    } else {
        throw std::runtime_error("Expected 'word' token");
    }
    while (_next_token.type == Token::Type::WORD) {
        parameters.push_back(*this->take_token(Token::Type::WORD).value);
    }

    if (_next_token.type == Token::Type::BLOCK_START) {
        this->take_token(Token::Type::BLOCK_START);

        while (_next_token.type != Token::Type::BLOCK_END) {
            children.push_back(parse_directive(name));
        }
        this->take_token(Token::Type::BLOCK_END);
    } else {
        if (parameters.size() < 1) {
            throw std::runtime_error("Expected at least one parameter");
        }

        this->take_token(Token::Type::END);
    }

    return Directive(name, parameters, children);
}
}  // namespace webserv::config
