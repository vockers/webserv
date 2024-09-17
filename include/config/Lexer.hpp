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

        /// @brief Returns a string representation of the token type.
        ///
        /// @param type The token type
        /// @return A string representation of the token type
        static const std::string type_to_string(Type type);

        /// @brief Returns the value of the token as `T`.
        ///
        /// @tparam T The type of the value
        /// @return The value of the token as `T`
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

    /// Scan word for `STRING`, `NUMBER` or `BOOL` token.
    Token scan_word();
    /// Skips all whitespaces (spaces, tabs, newlines, etc.)
    void skip_whitespaces();
    /// Is the character a delimiter (whitespace, '{', '}', ';')
    bool is_delimiter(char c);
};
}  // namespace webserv::config
