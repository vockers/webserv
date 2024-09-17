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

    /// @brief Get the value of a directive as `T` in the main directive
    ///
    /// @tparam T The type of the value
    /// @param type The type of the directive
    /// @param col The index of the parameter
    /// @return The value of the directive as `T`
    template <typename T>
    const T& get(Directive::Type type, size_t col) const
    {
        return _main_directive.get<T>(type, col);
    }

private:
    Directive _main_directive;
};
}  // namespace webserv::config
