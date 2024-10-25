#include <gtest/gtest.h>

#include "http/Request.hpp"
#include "http/Response.hpp"

using namespace webserv::http;
using StatusCode = Response::StatusCode;

#define EXPECT_THROW_VALUE(expr, type, exception) \
    try {                             \
        expr;                         \
        EXPECT_TRUE(false);           \
    } catch (type e) {                \
        EXPECT_EQ(e, exception);      \
    }

TEST(RequestTests, RequestTest)
{
    Request request("GET /index.html HTTP/1.1\r\n"
                    "Host: localhost:8080\r\n"
                    "User-Agent: curl/7.68.0\r\n"
                    "Accept: */*\r\n"
                    "\r\n");

    EXPECT_EQ(request.get_method(), Request::Method::GET);
    EXPECT_EQ(request.get_uri(), "/index.html");

    const auto& headers = request.get_headers();
    EXPECT_EQ(headers.size(), 3);
    EXPECT_EQ(headers.at("host"), "localhost:8080");
    EXPECT_EQ(headers.at("user-agent"), "curl/7.68.0");
    EXPECT_EQ(headers.at("accept"), "*/*");
}

TEST(RequestTests, InvalidMethodTest)
{
    EXPECT_THROW_VALUE(Request("INVALID /index.html HTTP/1.1\r\n\r\n"), StatusCode, StatusCode::NOT_IMPLEMENTED);
}

TEST(RequestTests, InvalidVersionTest)
{
    EXPECT_THROW_VALUE(Request("GET /index.html HTTP/1.0\r\n\r\n"), StatusCode, StatusCode::HTTP_VERSION_NOT_SUPPORTED);
}

TEST(RequestTests, BadRequestTest)
{
    // Missing CRLFCRLF
    EXPECT_THROW_VALUE(Request("GET /index.html HTTP/1.1"), StatusCode, StatusCode::BAD_REQUEST);
    // Missing CRLF
    EXPECT_THROW_VALUE(Request("GET /index.html HTTP/1.1\r\n"), StatusCode, StatusCode::BAD_REQUEST);
    // Missing Host header
    EXPECT_THROW_VALUE(Request("GET /index.html HTTP/1.1\r\n"
                               "User-Agent: curl/7.68.0\r\n"
                               "Accept: */*\r\n"
                               "\r\n"), StatusCode, StatusCode::BAD_REQUEST);
}

TEST(RequestTests, BodyTest)
{
    Request request("POST /index.html HTTP/1.1\r\n"
                    "Host: localhost:8080\r\n"
                    "User-Agent: curl/7.68.0\r\n"
                    "Accept: */*\r\n"
                    "Content-Length: 5\r\n"
                    "\r\n"
                    "Hello");

    EXPECT_EQ(request.content_length(), 5);
    EXPECT_EQ(request.body(), "Hello");
}
