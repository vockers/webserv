#include "utils/std_utils.hpp"

namespace webserv::utils
{
void free_string_array(char** array)
{
    for (size_t i = 0; array[i] != nullptr; ++i) {
        delete[] array[i];
    }
    delete[] array;
}
}  // namespace webserv::utils
