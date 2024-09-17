#pragma once

#include <optional>
#include <string>
#include <variant>

namespace webserv::config
{
class Lexer
{
public:
    struct Token
    {
        using Value = std::optional<std::variant<std::string, int, bool>>;

        enum class Type
        {
            STRING,
            NUMBER,
            BOOL,
            END,
            BLOCK_START,
            BLOCK_END,
            NONE,
        };
        Type  type;
        Value value;

        /// Returns a string representation of the token type.
        static const std::string type_to_string(Type type);

        template <typename T>
        T get()
        {
            return std::get<T>(*value);
        }
    };

    Lexer(const std::string& input);

    /// Returns the next token in the input.
    Token next_token();

private:
    std::string           _input;
    std::string::iterator _current;

    Token scan_word();
    /// Skips all whitespaces (spaces, tabs, newlines, etc.)
    void skip_whitespaces();
    /// Is the character a delimiter (whitespace, '{', '}', ';')
    bool is_delimiter(char c);
};
}  // namespace webserv::config
