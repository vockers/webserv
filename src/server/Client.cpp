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

    while (true) {
		int bytes_read = read(_fd, buffer.get(), BUFFER_SIZE);

        if (bytes_read == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No more data available to read right now
                return;  // Exit the loop to return control to the event loop
            }
            // Handle other read errors
            std::cerr << "Error reading from socket" << std::endl;
			perror("read");
            return;
        }

        if (bytes_read == 0) {
            // Connection closed by the client
            std::cout << "Client disconnected: " << _fd << std::endl;
			close(_fd); // Close the socket ?
            return;
        }

        // Store the data in your buffer (e.g., _buffer)
        _buffer.write(buffer.get(), bytes_read);  // Append to your buffer (preferably .write instead of << for raw data)
        std::cout << "Received from client " << _fd << ": [" << _buffer.str() << "]" << std::endl;
    }
}

void Client::handle_write() {
	std::cout << "handle_write() from client: " << _fd << std::endl;
	_buffer.str(""); // Clear the buffer after writing
	_buffer.clear();  // Clear the buffer flags after writing
}
}