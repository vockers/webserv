#pragma once

#include <string>
#include <vector>

#include "server/Listen.hpp"

class Server
{
public:
    Server(const std::string& name, const std::string& address, int port);
    ~Server();

    void run();

private:
    std::string _name;

    Listen _listen;

    int _epoll_fd;

    std::vector<Socket> _sockets;
};
