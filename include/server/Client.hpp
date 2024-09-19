#pragma once

#include <sstream>

#include "server/Socket.hpp"

namespace webserv::server
{
class Client : public Socket
{
public:
	Client(Socket&& socket);

	void handle_read();
	void handle_write();

private:
	std::stringstream _buffer;
};
}  // namespace webserv::server