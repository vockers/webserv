#include "config/Config.hpp"
#include "server/Server.hpp"
#include "utils/Logger.hpp"

using webserv::config::Config;
using webserv::config::Directive;
using webserv::server::Server;
using webserv::utils::ErrorLogger;

int main()
{
    ErrorLogger elog(ErrorLogger::INFO);

    try {
        Config config("conf/default.conf");
        
        elog.set_level(config.get<std::string>(Directive::LOG_LEVEL, 0));

        elog.log(ErrorLogger::INFO, "Starting webserv...");

        Server server("server", "0.0.0.0", 8080, elog);

        server.run();
    } catch (const std::exception& e) {
        elog.log(ErrorLogger::CRITICAL, e.what());
    }

    return 0;
}
