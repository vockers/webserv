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
            END,
            BLOCK_START,
            BLOCK_END,
            NONE,
        };
        Type                       type;
        std::optional<std::string> value;

        static const std::string type_to_string(Type type);
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
