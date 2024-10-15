#pragma once

#include <algorithm>

namespace webserv::utils
{
/// @brief Check if a container contains a value.
///
/// @tparam Container The type of the container.
/// @tparam T The type of the value.
/// @param container The container to check.
/// @param value The value to check.
/// @return True if the container contains the value, false otherwise.
template <typename Container, typename T>
bool contains(const Container& container, const T& value)
{
    return std::find(container.begin(), container.end(), value) != container.end();
}
}  // namespace webserv::utils
