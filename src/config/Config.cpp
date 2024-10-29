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
    {"error_page",           ERROR_PAGE},
    {"upload_dir",           UPLOAD_DIR}
};

// format: {{<allowed parents>, <unique>, [min params], [max params]}}
const Config::Constraint Config::CONSTRAINTS[] = {
    {{}},                                        // MAIN
    {{MAIN}, true, nullopt, 0},                  // HTTP
    {{HTTP}, false, nullopt, 0},                 // SERVER
    {{SERVER}, false, 1},                        // LOCATION
    {{SERVER}, false, 1},                        // SERVER_NAME
    {{SERVER}, false, 1, 2},                     // LISTEN
    {{HTTP, SERVER, LOCATION}, true, 1, 1},      // ROOT
    {{HTTP, SERVER, LOCATION}, true, 1},         // INDEX
    {{MAIN}, true, 1, 1},                        // LOG_LEVEL
    {{LOCATION}, true, 1, nullopt},              // LIMIT_EXCEPT
    {{HTTP, SERVER, LOCATION}, true, 1, 1},      // AUTOINDEX
    {{HTTP, SERVER, LOCATION}, true, 1, 1},      // CLIENT_MAX_BODY_SIZE
    {{HTTP, SERVER, LOCATION}, true, 1, 2},      // RETURN
    {{HTTP, SERVER, LOCATION}, false, 2},        // ERROR_PAGE
    {{HTTP, SERVER, LOCATION}, true, 1, 1}       // UPLOAD_DIR
};

const Config::Parameters Config::DEFAULT_PARAMS[] = {
    {},                // MAIN
    {},                // HTTP
    {},                // SERVER
    {},                // LOCATION
    {""},              // SERVER_NAME
    {80, "0.0.0.0"},   // LISTEN
    {"./www/default"}, // ROOT
    {"index.html"},    // INDEX
    {"info"},          // LOG_LEVEL
    {},                // LIMIT_EXCEPT
    {false},           // AUTOINDEX
    {1048576},         // CLIENT_MAX_BODY_SIZE (1MiB)
    {301},             // RETURN
    {},                // ERROR_PAGE
    {},                // UPLOAD_DIR
};
// clang-format on

Config::iterator::iterator(const Config* config, size_t index) : _config(config), _index(index) {}

Config::iterator& Config::iterator::next(Type type)
{
    while (*this != _config->end()) {
        ++_index;
        if (_index == _config->_children.size() || _config->_children[_index]->get_type() == type) {
            break;
        }
    }

    return *this;
}

Config::iterator& Config::iterator::operator++()
{
    ++_index;
    return *this;
}

Config::iterator& Config::iterator::operator++(int)
{
    ++_index;
    return *this;
}

const Config& Config::iterator::operator*()
{
    return *_config->_children[_index];
}

const Config* Config::iterator::operator->()
{
    return _config->_children[_index].get();
}

bool Config::iterator::operator==(const iterator& other) const
{
    return _config == other._config && _index == other._index;
}

bool Config::iterator::operator!=(const iterator& other) const
{
    return !(*this == other);
}

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

Config::ConfigList Config::find(Type type) const
{
    ConfigList list;

    for (const auto& child : _children) {
        if (child->get_type() == type) {
            list.push_back(*child.get());
        }
    }

    if (_parent) {
        ConfigList parent_list = _parent->find(type);
        list.insert(list.end(), parent_list.begin(), parent_list.end());
    }

    return list;
}

Config::iterator Config::begin() const
{
    return Config::iterator(this, 0);
}

Config::iterator Config::begin(Type type) const
{
    for (size_t i = 0; i < _children.size(); ++i) {
        if (_children[i]->get_type() == type) {
            return Config::iterator(this, i);
        }
    }

    return Config::iterator(this, _children.size());
}

Config::iterator Config::end() const
{
    return Config::iterator(this, _children.size());
}

const Config& Config::location(const std::string& uri) const
{
    for (auto it = this->begin(Type::LOCATION); it != this->end(); it = it.next(Type::LOCATION)) {
        for (const auto& param : it->get_parameters()) {
            std::string param_name = std::get<std::string>(param);
            if (param_name == uri || (param_name.ends_with('/') && uri.starts_with(param_name))) {
                return *it;
            }
        }
    }

    return *this;
}

const std::string& Config::server_name() const
{
    return this->value<std::string>(SERVER_NAME, 0);
}

const std::string& Config::host() const
{
    return this->value<std::string>(LISTEN, 1);
}

const std::string& Config::root() const
{
    return this->value<std::string>(ROOT, 0);
}

const std::string& Config::index() const
{
    return this->value<std::string>(INDEX, 0);
}

const std::string& Config::log_level() const
{
    return this->value<std::string>(LOG_LEVEL, 0);
}

const std::string& Config::error_page(int code) const
{
    ConfigList error_pages = this->find(ERROR_PAGE);

    for (const Config& error_page : error_pages) {
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
    }

    throw std::runtime_error("Error page not found");
}

const std::string& Config::return_uri() const
{
    const static std::string empty = "";

    auto return_it = this->begin(Type::RETURN);
    if (return_it == this->end()) {
        return empty;
    }
    return std::get<std::string>(return_it->_parameters.at(return_it->_parameters.size() - 1));
}

const std::string& Config::upload_dir() const
{
    return this->value<std::string>(UPLOAD_DIR, 0);
}

int Config::port() const
{
    return this->value<int>(LISTEN, 0);
}

bool Config::limit_except(const std::string& method) const
{
    const Config* config = this->get(Type::LIMIT_EXCEPT);
    if (config == nullptr) {
        return true;
    }

    for (const auto& param : config->get_parameters()) {
        if (std::get<std::string>(param) == method) {
            return true;
        }
    }

    return false;
}

bool Config::autoindex() const
{
    return this->value<bool>(AUTOINDEX, 0);
}

int Config::client_max_body_size() const
{
    return this->value<int>(CLIENT_MAX_BODY_SIZE, 0);
}

int Config::return_code() const
{
    auto return_it = this->begin(Type::RETURN);
    if (return_it == this->end() || return_it->_parameters.size() == 1) {
        return std::get<int>(DEFAULT_PARAMS[static_cast<int>(Type::RETURN)].at(0));
    }
    return std::get<int>(return_it->_parameters.at(0));
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
