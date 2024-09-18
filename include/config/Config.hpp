#pragma once

#include "config/Directive.hpp"

namespace webserv::config
{
class Config : public Directive
{
public:
    Config(const std::string& file_path);

    using Directive::operator=;
};
}  // namespace webserv::config
