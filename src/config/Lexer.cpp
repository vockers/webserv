#include "config/Lexer.hpp"

namespace webserv::config
{
using Token = Lexer::Token;

const std::string Token::type_to_string(Token::Type type)
{
    switch (type) {
    case Token::Type::STRING:
        return "string";
        break;
    case Token::Type::NUMBER:
        return "number";
        break;
    case Token::Type::BOOL:
        return "bool";
        break;
    case Token::Type::END:
        return ";";
        break;
    case Token::Type::BLOCK_START:
        return "{";
        break;
    case Token::Type::BLOCK_END:
        return "}";
        break;
    case Token::Type::NONE:
        return "EOF";
        break;
    }
}

Lexer::Lexer(const std::string& input) : _input(input), _current(_input.begin()) {}

Token Lexer::next_token()
{
    skip_whitespaces();

    if (_current == _input.end()) {
        return {Token::Type::NONE, ""};
    }

    switch (*_current) {
    case ';':
        ++_current;
        return {Token::Type::END, {}};
    case '{':
        ++_current;
        return {Token::Type::BLOCK_START, {}};
    case '}':
        ++_current;
        return {Token::Type::BLOCK_END, {}};
    default:
        return scan_word();
    }
}

Token Lexer::scan_word()
{
    std::string word;
    Token::Type type = Token::Type::NUMBER;
    while (_current != _input.end() && !is_delimiter(*_current)) {
        if (!std::isdigit(*_current)) {
            type = Token::Type::STRING;
        }
        word += *_current;
        ++_current;
    }
    if (type == Token::Type::NUMBER) {
        return {Token::Type::NUMBER, std::stoi(word)};
    } else if (word == "on" || word == "off") {
        return {Token::Type::BOOL, word == "on" ? true : false};
    }
    return {Token::Type::STRING, word};
}

void Lexer::skip_whitespaces()
{
    while (_current != _input.end() && std::isspace(*_current)) {
        ++_current;
    }
}

bool Lexer::is_delimiter(char c)
{
    return std::isspace(c) || c == '{' || c == '}' || c == ';';
}
}  // namespace webserv::config
