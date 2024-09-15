#include "config/Directive.hpp"

#include <algorithm>
#include <stdexcept>

namespace webserv::config
{
Directive::Directive() : _name(""), _parameters(), _children() {}

Directive::Directive(const std::string& name, const Keys& parameters, const Directives& children)
    : _name(name), _parameters(parameters), _children(children)
{
}

const std::string& Directive::get_name() const
{
    return _name;
}

const Directive::Keys& Directive::get_parameters() const
{
    return _parameters;
}

const Directive::Directives& Directive::get_children() const
{
    return _children;
}

void Directive::validate(const std::string& parent, const Directives& siblings) const
{
    // clang-format off
    // format: {<name>, {<allowed children>, <unique>}}
    static const Constraint::Constraints constraints = {
        {"",                     {{"http", "log_level"}}},
        {"http",                 {{"autoindex", "client_max_body_size",
                                   "error_page", "index", "root", "server", "upload_dir"}, true}},
        {"server",               {{"autoindex", "client_max_body_size",
                                   "error_page", "index", "listen", "location",
                                   "return", "root", "server_name", "upload_dir"}}},
        {"location",             {{"autoindex", "client_max_body_size",
                                   "error_page", "index", "limit_except",
                                   "location", "return", "root", "upload_dir"}}},
        {"autoindex",            {{}, true}},
        {"client_max_body_size", {{}, true}},
        {"http",                 {{}, true}},
        {"index",                {{}, true}},
        {"limit_except",         {{}, true}},
        {"log_level",            {{}, true}},
        {"return",               {{}, true}},
        {"root",                 {{}, true}}
    };
    // clang-format on

    // Check if the directive is allowed in the parent directive
    auto it = constraints.find(parent);
    if (it == constraints.end() ||
        std::find(it->second.children.begin(), it->second.children.end(), _name) ==
            it->second.children.end()) {
        throw std::runtime_error("Directive '" + _name + "' is not allowed in '" + parent + "'");
    }

    // Check if the directive is unique
    for (const auto& d : siblings) {
        if (d.get_name() == _name) {
            throw std::runtime_error("Directive '" + _name + "' is not unique");
        }
    }
}
}  // namespace webserv::config
