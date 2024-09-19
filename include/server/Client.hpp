#pragma once

#include "server/Socket.hpp"

namespace webserv::server
{
class Client
{
public:
	Client(Socket&& socket);

	void on_read();
	void on_write();

private:
	Socket _socket;
};
}  // namespace webserv::server