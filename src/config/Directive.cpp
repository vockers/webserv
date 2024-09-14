#include "config/Directive.hpp"

namespace webserv::config
{
Directive::Directive() : _name(""), _parameters(), _children() {}

Directive::Directive(std::string name, Parameters parameters, Directives children)
    : _name(std::move(name)), _parameters(std::move(parameters)), _children(std::move(children))
{
}

const std::string& Directive::get_name() const
{
    return _name;
}

const Directive::Parameters& Directive::get_parameters() const
{
    return _parameters;
}

const Directive::Directives& Directive::get_children() const
{
    return _children;
}
}  // namespace webserv::config
