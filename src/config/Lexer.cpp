#include "config/Lexer.hpp"

namespace config
{
using Token = Lexer::Token;

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
        return {Token::Type::RULE_END, {}};
    case '{':
        ++_current;
        return {Token::Type::BLOCK_START, {}};
    case '}':
        ++_current;
        return {Token::Type::BLOCK_END, {}};
    default:
        std::string word;
        while (_current != _input.end() && !is_delimiter(*_current)) {
            word += *_current;
            ++_current;
        }
        return {Token::Type::WORD, word};
    }
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
}  // namespace config
