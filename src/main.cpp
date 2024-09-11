#include "server/Server.hpp"

int main()
{
    Server server("server", "0.0.0.0", 8080);

    server.run();

    return 0;
}
