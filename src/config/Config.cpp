#include "config/Config.hpp"

#include <fstream>
#include <sstream>

#include "config/Parser.hpp"

namespace webserv::config
{
Config::Config(const std::string& file_path)
{
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + file_path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    Parser parser(buffer.str());
    _main_directive = parser.parse();
}

const Directive& Config::get_main_directive() const
{
    return _main_directive;
}
}  // namespace webserv::config
