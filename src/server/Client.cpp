#include "server/Client.hpp"

#include <iostream>
#include <unistd.h>
#include <memory>

#define BUFFER_SIZE 4096

namespace webserv::server
{
Client::Client(Socket&& socket) : Socket(std::move(socket)) {}

void Client::handle_read() {
	std::unique_ptr<char[]> buffer(new char[BUFFER_SIZE] {0});

	int bytes_read = read(_fd, buffer.get(), BUFFER_SIZE);

	if (bytes_read == -1) {
		std::cerr << "Error reading from socket" << std::endl << _fd << std::endl;
		perror("read");
		return;
	}

	std::cout << buffer.get() << std::endl;
	std::cout << "Hello World (on_read baby)!" << std::endl;
}

void Client::handle_write() {
	std::cout << "Hello World (on_write baby)!" << std::endl;
}
}