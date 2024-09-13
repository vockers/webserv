#pragma once

#include <optional>
#include <string>

namespace webserv::config
{
class Lexer
{
public:
    struct Token
    {
        enum class Type
        {
            WORD,
            RULE_END,
            BLOCK_START,
            BLOCK_END,
            NONE,
        };
        Type                       type;
        std::optional<std::string> value;
    };

    Lexer(const std::string& input);

    Token next_token();

private:
    std::string           _input;
    std::string::iterator _current;

    void skip_whitespaces();
    bool is_delimiter(char c);
};
}  // namespace webserv::config
