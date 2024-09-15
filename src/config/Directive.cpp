#include "config/Directive.hpp"

#include <optional>
#include <stdexcept>

#include "utils/std_utils.hpp"

using std::nullopt;

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

const Directive::Parameters& Directive::get_parameters() const
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
    // format: {<name>, {<allowed children>, <unique>, [min params], [max params]}}
    static const Constraint::Constraints constraints = {
        {"",                     {{"http", "log_level"}}},
        {"http",                 {{"autoindex", "client_max_body_size",
                                   "error_page", "index", "root", "server", "upload_dir"}, true, nullopt, 0}},
        {"server",               {{"autoindex", "client_max_body_size",
                                   "error_page", "index", "listen", "location",
                                   "return", "root", "server_name", "upload_dir"}, false, nullopt, 0}},
        {"location",             {{"autoindex", "client_max_body_size",
                                   "error_page", "index", "limit_except",
                                   "location", "return", "root", "upload_dir"}, false, 1, nullopt}},
        {"log_level",            {{}, true, 1, 1, {"debug", "info", "warn", "error"}}},
        {"limit_except",         {{}, true, 1, nullopt, {"GET", "POST", "DELETE"}}},
        {"autoindex",            {{}, true, 1, 1, {"on", "off"}}},
        {"client_max_body_size", {{}, true, 1, 1}},
        {"return",               {{}, true, 1, 2}},
        {"root",                 {{}, true, 1, 1}},
        {"error_page",           {{}, false, 2}},
        {"listen",               {{}, false, 1}},
        {"server_name",          {{}, false, 1}},
        {"index",                {{}, true, 1}}
    };
    // clang-format on

    // Check if the directive is allowed
    auto it = constraints.find(_name);
    if (it == constraints.end()) {
        throw std::runtime_error("Directive '" + _name + "' is not allowed");
    }
    const auto& constraint = it->second;

    // Check if the directive is allowed in the parent directive
    auto parent_it = constraints.find(parent);
    if (parent_it == constraints.end()) {
        throw std::runtime_error("Directive '" + parent + "' is not allowed");
    }
    const auto& parent_constraint = parent_it->second;
    if (!utils::contains(parent_constraint.children, _name)) {
        throw std::runtime_error("Directive '" + _name + "' is not allowed in '" + parent + "'");
    }

    // Check if the directive has the correct number of parameters
    if (constraint.min_params.has_value() && _parameters.size() < *constraint.min_params) {
        throw std::runtime_error("Directive '" + _name + "' requires at least " +
                                 std::to_string(*constraint.min_params) + " parameters");
    }
    if (constraint.max_params.has_value() && _parameters.size() > *constraint.max_params) {
        throw std::runtime_error("Directive '" + _name + "' requires at most " +
                                 std::to_string(*it->second.max_params) + " parameters");
    }

    // Check if the directive is unique
    if (constraint.unique) {
        for (const auto& d : siblings) {
            if (d.get_name() == _name) {
                throw std::runtime_error("Directive '" + _name + "' is not unique");
            }
        }
    }

    // Check if the parameters are allowed
    if (!constraint.allowed_params.empty()) {
        for (const auto& param : _parameters) {
            if (!utils::contains(constraint.allowed_params, param)) {
                throw std::runtime_error("Parameter '" + param +
                                         "' is not allowed for directive '" + _name + "'");
            }
        }
    }
}
}  // namespace webserv::config
