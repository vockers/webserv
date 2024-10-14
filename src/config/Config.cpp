#include "config/Config.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>

#include "config/Parser.hpp"

using std::nullopt;

namespace webserv::config
{
using Type = Config::Type;

// clang-format off
const std::map<std::string, Type> Config::TYPE_MAP = {
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
const Config::Constraint Config::CONSTRAINTS[] = {
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

const Config::Parameters Config::DEFAULT_PARAMS[] = {
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

Config::Config(const std::string& file_path) : _name(""), _type(MAIN), _parent(nullptr)
{
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + file_path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    Parser parser(buffer.str());
    parser.parse(*this);
}

Config::Config(const std::string& name, Type type, Config* parent)
    : _name(name), _type(type), _parent(parent)
{
}

const Config* Config::get(Type type) const
{
    for (const auto& child : _children) {
        if (child->get_type() == type) {
            return child.get();
        }
    }

    if (_parent != nullptr) {
        std::cout << "Parent: " << _parent->get_type() << std::endl;
        return _parent->get(type);
    }

    return nullptr;
}

const Config& Config::operator[](Type type) const
{
    for (const auto& child : _children) {
        if (child->get_type() == type) {
            return *child.get();
        }
    }

    if (_parent) {
        return (*_parent)[type];
    }

    throw std::runtime_error("Config not found");
}

const std::string& Config::server_name() const
{
    return this->value<std::string>(SERVER_NAME, 0);
}

const std::string& Config::log_level() const
{
    return this->value<std::string>(LOG_LEVEL, 0);
}

const std::string& Config::error_page(int code) const
{
    const Config& error_page = (*this)[ERROR_PAGE];
    if (error_page.get_parameters().size() == 1)
        return this->value<std::string>(ERROR_PAGE, 0);

    for (auto param = error_page.get_parameters().begin();
         param != error_page.get_parameters().end() - 1;
         ++param) {
        if (std::get<int>(*param) == code) {
            return std::get<std::string>(
                error_page.get_parameters().at(error_page.get_parameters().size() - 1));
        }
    }

    throw std::runtime_error("Error page not found");
}

int Config::listen() const
{
    return this->value<int>(LISTEN, 0);
}

Type Config::get_type() const
{
    return _type;
}

const std::string& Config::get_name() const
{
    return _name;
}

const Config::Parameters& Config::get_parameters() const
{
    return _parameters;
}

const Config::Directives& Config::get_children() const
{
    return _children;
}

const Config* Config::get_parent() const
{
    return _parent;
}

void Config::add_parameter(const Value& value)
{
    _parameters.push_back(value);
}

void Config::add_child(std::shared_ptr<Config> child)
{
    _children.push_back(child);
}

const Config::Constraint& Config::get_constraint(Type type)
{
    return CONSTRAINTS[static_cast<int>(type)];
}

const Config::Parameters& Config::get_default_params(Type type)
{
    return DEFAULT_PARAMS[static_cast<int>(type)];
}
}  // namespace webserv::config
