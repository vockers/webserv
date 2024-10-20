#pragma once

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace webserv::config
{
class Config
{
public:
    using Value      = std::variant<std::string, int, bool>;
    using Parameters = std::vector<Value>;
    using Directives = std::vector<std::shared_ptr<Config>>;
    using ConfigList = std::vector<std::reference_wrapper<Config>>;

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

    Config(const std::string& file_path);
    Config(const std::string& name, Type type, Config* parent = nullptr);

    /// @brief Get the directive of a specific type
    ///
    /// Searches for a directive of the specified type in the children of this directive.
    /// If it doesn't exist, it will search in the parent of this directive, and so on.
    ///
    /// @param type The type of the directive
    /// @return The directive of the specified type or `nullptr` if it doesn't exist
    const Config* get(Type type) const;

    /// @brief Get the directive of a specific type
    ///
    /// Searches for a directive of the specified type in the children of this directive.
    /// If it doesn't exist, it will search in the parent of this directive, and so on.
    ///
    /// @param type The type of the directive
    /// @return The directive of the specified type as reference
    const Config& operator[](Type type) const;

    /// @brief Find all directives of a specific type
    ///
    /// Searches for all directives of the specified type in the children
    /// of this directive and parent.
    ///
    /// @param type The type of the directive
    /// @return A list of all directives of the specified type
    ConfigList find(Type type) const;

    /// @brief Get the value of a directive as `T`
    ///
    /// @tparam T The type of the value
    /// @param type The type of the directive
    /// @param col The index of the parameter
    /// @return The value of the directive as `T`
    template <typename T>
    const T& value(Type type, size_t col) const
    {
        const Config* directive = this->get(type);

        if (directive == nullptr) {
            if (get_default_params(type).size() <= col) {
                throw std::runtime_error("No default value for this directive");
            }
            return std::get<T>(get_default_params(type)[col]);
        }

        if (directive->get_parameters().size() <= col) {
            throw std::runtime_error("No value for this directive");
        }

        return std::get<T>(directive->get_parameters()[col]);
    }

    const Config& location(const std::string& uri) const;

    const std::string& server_name() const;
    const std::string& root() const;
    const std::string& index() const;
    const std::string& log_level() const;
    const std::string& error_page(int code) const;

    int  listen() const;
    bool autoindex() const;
    int  client_max_body_size() const;

    Type               get_type() const;
    const std::string& get_name() const;
    const Parameters&  get_parameters() const;
    const Directives&  get_children() const;
    const Config*      get_parent() const;

    void add_parameter(const Value& value);
    void add_child(std::shared_ptr<Config> child);

    static const std::map<std::string, Type> TYPE_MAP;
    static const Constraint                  CONSTRAINTS[];
    static const Parameters                  DEFAULT_PARAMS[];

    static const Constraint& get_constraint(Type type);
    static const Parameters& get_default_params(Type type);

protected:
    std::string _name;
    Type        _type;
    Parameters  _parameters;
    Directives  _children;
    Config*     _parent;
};
}  // namespace webserv::config
