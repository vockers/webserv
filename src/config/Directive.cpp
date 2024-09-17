#include "config/Directive.hpp"

#include <memory>
#include <optional>

using std::nullopt;

namespace webserv::config
{
using Type = Directive::Type;

// clang-format off
const std::map<std::string, Type> Directive::TYPE_MAP = {
    {"",                     MAIN},
    {"http",                 HTTP},
    {"server",               SERVER},
    {"location",             LOCATION},
    {"server_name",          SERVER_NAME},
    {"listen",               LISTEN},
    {"root",                 ROOT},
    {"index",                INDEX},
    {"log_level",            LOG_LEVEL},
    {"limit_except",         LIMIT_EXCEPT},
    {"autoindex",            AUTOINDEX},
    {"client_max_body_size", CLIENT_MAX_BODY_SIZE},
    {"return",               RETURN},
    {"error_page",           ERROR_PAGE}
};

// format: {{<allowed parents>, <unique>, [min params], [max params]}}
const Directive::Constraint Directive::CONSTRAINTS[] = {
    {{}},                                        // MAIN
    {{MAIN}, true, nullopt, 0},                  // HTTP
    {{HTTP}, false, nullopt, 0},                 // SERVER
    {{SERVER, LOCATION}, false, 1},              // LOCATION
    {{SERVER}, false, 1},                        // SERVER_NAME
    {{SERVER}, false, 1},                        // LISTEN
    {{HTTP, SERVER, LOCATION}, true, 1, 1},      // ROOT
    {{HTTP, SERVER, LOCATION}, true, 1},         // INDEX
    {{MAIN}, true, 1, 1},                        // LOG_LEVEL
    {{LOCATION}, true, 1, nullopt},              // LIMIT_EXCEPT
    {{HTTP, SERVER, LOCATION}, true, 1, 1},      // AUTOINDEX
    {{HTTP, SERVER, LOCATION}, true, 1, 1},      // CLIENT_MAX_BODY_SIZE
    {{HTTP, SERVER, LOCATION}, true, 1, 2},      // RETURN
    {{HTTP, SERVER, LOCATION}, false, 2}         // ERROR_PAGE
};

const Directive::Parameters Directive::DEFAULT_PARAMS[] = {
    {},                // MAIN
    {},                // HTTP
    {},                // SERVER
    {},                // LOCATION
    {""},              // SERVER_NAME
    {80},              // LISTEN
    {"./www/default"}, // ROOT
    {"index.html"},    // INDEX
    {"info"},          // LOG_LEVEL
    {},                // LIMIT_EXCEPT
    {false},           // AUTOINDEX
    {1048576},         // CLIENT_MAX_BODY_SIZE (1MiB)
    {},                // RETURN
    {}                 // ERROR_PAGE
};
// clang-format on

Directive::Directive(const std::string& name, Type type, Directive* parent)
    : _name(name), _type(type), _parent(parent)
{
}

const Directive* Directive::operator[](Type type) const
{
    for (const auto& child : _children) {
        if (child->get_type() == type) {
            return child.get();
        }
    }

    if (_parent) {
        return (*_parent)[type];
    }

    return nullptr;
}

const std::string& Directive::get_name() const
{
    return _name;
}

Type Directive::get_type() const
{
    return _type;
}

const Directive::Parameters& Directive::get_parameters() const
{
    return _parameters;
}

const Directive::Directives& Directive::get_children() const
{
    return _children;
}

const Directive* Directive::get_parent() const
{
    return _parent;
}

void Directive::add_parameter(const Value& value)
{
    _parameters.push_back(value);
}

void Directive::add_child(std::shared_ptr<Directive> child)
{
    _children.push_back(child);
}

const Directive::Constraint& Directive::get_constraint(Type type)
{
    return CONSTRAINTS[static_cast<int>(type)];
}

const Directive::Parameters& Directive::get_default_params(Type type)
{
    return DEFAULT_PARAMS[static_cast<int>(type)];
}
}  // namespace webserv::config
