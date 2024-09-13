#pragma once

#include "config/Directive.hpp"

namespace webserv::config
{
class Config
{
public:
    Config(Directive main_directive);

    const Directive& get_main_directive() const;

private:
    Directive _main_directive;
};
}  // namespace webserv::config
