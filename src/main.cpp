#include "config/Config.hpp"
#include "net/Server.hpp"
#include "utils/Logger.hpp"

using webserv::config::Config;
using webserv::net::Server;
using webserv::utils::ErrorLogger;

int main()
{
    ErrorLogger elog(ErrorLogger::INFO);

    try {
        Config config("conf/default.conf");

        elog.set_level(config.log_level());

        elog.log(ErrorLogger::INFO, "Starting webserv...");

        Server server(config[Config::HTTP][Config::SERVER], elog);

        server.run();
    } catch (const std::exception& e) {
        elog.log(ErrorLogger::CRITICAL, e.what());
    }

    return 0;
}
