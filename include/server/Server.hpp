#pragma once

#include <string>

#include "server/Listen.hpp"

class Server
{
private:
    std::string _name;

    Listen _listen;
};
