#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace webserv::config
{
class Directive
{
public:
    using Keys       = std::vector<std::string>;
    using Parameters = std::vector<std::string>;
    using Directives = std::vector<Directive>;

    /// Used for validation
    struct Constraint
    {
        using Constraints = std::map<std::string, Constraint>;

        Keys children = {};
        bool unique   = false;

        std::optional<size_t> min_params = std::nullopt;
        std::optional<size_t> max_params = std::nullopt;

        Parameters allowed_params = {};
    };

    Directive();
    Directive(const std::string& name, const Keys& parameters, const Directives& children);

    const std::string& get_name() const;
    const Parameters&  get_parameters() const;
    const Directives&  get_children() const;

    /// Validate the directive based certain constraints
    ///
    /// @param parent The parent directive
    /// @param siblings The siblings of the directive
    void validate(const std::string& parent, const Directives& siblings) const;

private:
    std::string _name;
    Parameters  _parameters;
    Directives  _children;
};
}  // namespace webserv::config
