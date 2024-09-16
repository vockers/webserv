#include "server/Server.hpp"
#include "utils/Logger.hpp"

using webserv::server::Server;
using webserv::utils::ErrorLogger;

int main()
{
    ErrorLogger elog(ErrorLogger::DEBUG);

    elog.log(ErrorLogger::INFO, "Starting webserv...");

    try {
        Server server("server", "0.0.0.0", 8080, elog);

        server.run();
    } catch (const std::exception& e) {
        elog.log(ErrorLogger::CRITICAL, e.what());
    }

    return 0;
}
