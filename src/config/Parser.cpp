#include "config/Parser.hpp"

#include <stdexcept>

#include "config/Directive.hpp"
#include "utils/std_utils.hpp"

namespace webserv::config
{
using Type = Directive::Type;

Parser::Parser(const std::string& input) : _lexer(input)
{
    _next_token = _lexer.next_token();
}

Directive Parser::parse()
{
    Directive main(Type::MAIN);

    while (_next_token.type != Token::Type::NONE) {
        main.add_child(this->parse_directive(main.get_children(), &main));
    }

    return main;
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

Directive Parser::parse_directive(const Directive::Directives& siblings, Directive* parent)
{
    std::string name = *this->take_token(Token::Type::WORD).value;

    // Check if the directive is known
    auto it = Directive::TYPE_MAP.find(name);
    if (it == Directive::TYPE_MAP.end()) {
        throw std::runtime_error("Unknown directive: " + name);
    }
    Directive directive(it->second, parent);

    // Check if the directive is allowed in the parent directive
    const auto& constraint = Directive::CONSTRAINTS[static_cast<int>(directive.get_type())];
    if (!utils::contains(constraint.parents, parent->get_type())) {
        throw std::runtime_error("Directive '" + name + "' is not allowed in this context");
    }

    while (_next_token.type == Token::Type::WORD) {
        directive.add_parameter(*this->take_token(Token::Type::WORD).value);
    }

    // Check if the directive has the correct number of parameters
    if (constraint.min_params.has_value() &&
        directive.get_parameters().size() < *constraint.min_params) {
        throw std::runtime_error("directive '" + name + "' requires at least " +
                                 std::to_string(*constraint.min_params) + " parameters");
    }
    if (constraint.max_params.has_value() &&
        directive.get_parameters().size() > *constraint.max_params) {
        throw std::runtime_error("Directive '" + name + "' requires at most " +
                                 std::to_string(*constraint.max_params) + " parameters");
    }

    // Check if the directive is unique
    if (constraint.unique) {
        for (const auto& d : siblings) {
            if (d.get_type() == directive.get_type()) {
                throw std::runtime_error("Directive '" + name + "' is not unique in this context");
            }
        }
    }

    if (_next_token.type == Token::Type::BLOCK_START) {
        this->take_token(Token::Type::BLOCK_START);

        while (_next_token.type != Token::Type::BLOCK_END) {
            directive.add_child(parse_directive(directive.get_children(), &directive));
        }
        this->take_token(Token::Type::BLOCK_END);
    } else {
        this->take_token(Token::Type::END);
    }

    return directive;
}
}  // namespace webserv::config
