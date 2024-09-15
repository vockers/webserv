#pragma once

#include <algorithm>

namespace webserv::utils
{
template <typename Container, typename T>
bool contains(const Container& container, const T& value)
{
    return std::find(container.begin(), container.end(), value) != container.end();
}
}  // namespace webserv::utils
