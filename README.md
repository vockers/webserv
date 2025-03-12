# webserv

A C++ school project for 42 Codam

## Overview

`webserv` is a lightweight non-blocking HTTP/1.1 web server written in C++. It uses Nginx-like configuration syntax. It makes use of `epoll` for the event management

## Features

- Supports HTTP/1.1
- Nginx-like configuration syntax
- CGI (currently only python)
- Custom error pages
- File uploads
- Autoindex (directory listing)

## Example Configuration

```nginx
log_level debug;

http {
    server {
        listen 8080;
        server_name localhost;

        error_page 404 /error/404.html;
        error_page 400 505 /error/400.html;

        location /upload/ {
            root www/default;
            upload_dir www/default/upload/;
            autoindex on;
        }

        location / {
            root www/default;
            index index.html;
            autoindex on;
        }
    }
}
```

## Build Instructions

To build in debug mode:

```sh
make debug
```

For a release build:

```sh
make release
```

## Running the Server

After building, you can start the server with:

```sh
./webserv
```

By default, it will look for a configuration file. You can specify a custom configuration file:

```sh
./webserv path/to/config.conf
```

## Testing

To run unit tests using Docker:

```sh
make test
```
To run E2E tests:

```sh
make e2e
```

## Cleaning Up

To remove compiled objects:

```sh
make clean
```

To remove all build artifacts:

```sh
make fclean
```

To rebuild everything from scratch:

```sh
make re
```

## Code Formatting

To format the codebase using `clang-format`:

```sh
make format
```
