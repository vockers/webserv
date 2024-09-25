#include "server/Client.hpp"

#include <unistd.h>

#include <iostream>
#include <memory>

#define BUFFER_SIZE 4096

namespace webserv::server
{
Client::Client(Socket&& socket, ErrorLogger& elog) : Socket(std::move(socket)), _elog(elog) {}

void Client::handle_read()
{
    std::unique_ptr<char[]> buffer(new char[BUFFER_SIZE]{0});

    while (true) {
        ssize_t bytes_read = read(_fd, buffer.get(), BUFFER_SIZE);

        if (bytes_read == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                _elog.log(ErrorLogger::DEBUG, "No more data available to read right now");
                // No more data available to read right now
                break;  // Exit the loop to return control to the event loop
            }
            // Handle other read errors
            _elog.log(ErrorLogger::ERROR, "Error reading from socket");
            return;
        }

        else if (bytes_read == 0) {
            // Connection closed by the client
            _elog.log(ErrorLogger::INFO, "Client disconnected from " + get_address().to_string());
            return;
        }

        // Store the data in your buffer (e.g., _buffer)
        _buffer.write(buffer.get(), bytes_read);
        _elog.log(
            ErrorLogger::DEBUG,
            "Bytes received from " + get_address().to_string() + ": " + std::to_string(bytes_read));
    }
}

Client::~Client()
{
    _elog.log(ErrorLogger::INFO, "Client disconnected from: " + _address.to_string());
}

void Client::handle_write()
{
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";
    ssize_t     len      = response.size();

    while (len > 0) {
        ssize_t bytes_written = write(_fd, response.c_str(), response.size());

        if (bytes_written == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                _elog.log(ErrorLogger::DEBUG,
                          "Socket buffer full, can't write more data right now");
                return;  // Exit the loop to return control to the event loop
            }
            // Handle other write errors
            _elog.log(ErrorLogger::ERROR, "Error writing to socket");
            return;
        }
        len -= bytes_written;
        _elog.log(
            ErrorLogger::DEBUG,
            "Bytes written to " + get_address().to_string() + ": " + std::to_string(bytes_written));
    }
    _buffer.str("");  // Clear the buffer after writing
}
}  // namespace webserv::server