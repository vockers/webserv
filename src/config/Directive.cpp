#include "config/Directive.hpp"

#include <optional>

using std::nullopt;

namespace webserv::config
{
using Type = Directive::Type;

// clang-format off
const std::map<std::string, Directive::Type> Directive::TYPE_MAP = {
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
// clang-format on

Directive::Directive(Type type, Directive* parent) : _type(type), _parent(parent) {}

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

void Directive::add_child(const Directive& child)
{
    _children.push_back(child);
}
}  // namespace webserv::config
