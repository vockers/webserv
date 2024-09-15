#include "server/Server.hpp"
#include "utils/Logger.hpp"

using webserv::server::Server;
using webserv::utils::Logger;

int main()
{
    LOG_INFO("Starting webserv...");

    Server server("server", "0.0.0.0", 8081);

    server.run();

    return 0;
}
