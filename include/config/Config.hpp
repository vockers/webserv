#pragma once

#include "config/Directive.hpp"

namespace webserv::config
{
class Config
{
public:
    Config(const std::string& file_path);

    /// Returns the main ("") directive of the configuration.
    const Directive& get_main_directive() const;

private:
    Directive _main_directive;
};
}  // namespace webserv::config
