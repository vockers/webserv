#pragma once

#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace webserv::config
{
class Directive
{
public:
    using Value      = std::variant<std::string, int, bool>;
    using Parameters = std::vector<Value>;
    using Directives = std::vector<Directive>;

    enum Type : int
    {
        MAIN,
        HTTP,
        SERVER,
        LOCATION,
        SERVER_NAME,
        LISTEN,
        ROOT,
        INDEX,
        LOG_LEVEL,
        LIMIT_EXCEPT,
        AUTOINDEX,
        CLIENT_MAX_BODY_SIZE,
        RETURN,
        ERROR_PAGE,
    };

    /// Used for validation
    struct Constraint
    {
        std::vector<Type> parents = {};
        bool              unique  = false;

        std::optional<size_t> min_params = std::nullopt;
        std::optional<size_t> max_params = std::nullopt;

        Parameters allowed_params = {};
    };

    Directive(const std::string& name, Type type, Directive* parent = nullptr);

    const std::string& get_name() const;
    Type               get_type() const;
    const Parameters&  get_parameters() const;
    const Parameters&  get_parameters(Type type) const;
    const Directives&  get_children() const;
    const Directive*   get_parent() const;

    void add_parameter(const Value& value);
    void add_child(const Directive& child);

    static const std::map<std::string, Type> TYPE_MAP;
    static const Constraint                  CONSTRAINTS[];
    static const Parameters                  DEFAULT_PARAMS[];

    static const Constraint& get_constraint(Type type);
    static const Parameters& get_default_params(Type type);

private:
    std::string _name;
    Type        _type;
    Parameters  _parameters;
    Directives  _children;
    Directive*  _parent;
};
}  // namespace webserv::config
