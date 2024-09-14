#pragma once

#include <string>
#include <vector>

namespace webserv::config
{
class Directive
{
public:
    using Parameters = std::vector<std::string>;
    using Directives = std::vector<Directive>;

    Directive(std::string name, Parameters parameters, Directives children);

    const std::string& get_name() const;
    const Parameters&  get_parameters() const;
    const Directives&  get_children() const;

private:
    std::string _name;
    Parameters  _parameters;
    Directives  _children;
};
}  // namespace webserv::config
