#include <gtest/gtest.h>

#include "config/Parser.hpp"

using namespace webserv::config;
using Type = Config::Type;

TEST(ParserTests, ParseEmptyConfig)
{
    Parser parser("");
    Config config("", Config::MAIN);
    parser.parse(config);

    EXPECT_EQ(config.get_type(), Config::MAIN);
    EXPECT_EQ(config.get_parameters().size(), 0);
    EXPECT_EQ(config.get_children().size(), 0);
}

TEST(ParserTests, ParseSingleDirective)
{
    Parser parser("log_level debug;");
    Config config("", Config::MAIN);
    parser.parse(config);

    auto log_level = *config.get_children()[0];

    EXPECT_EQ(log_level.get_type(), Config::LOG_LEVEL);
    EXPECT_EQ(log_level.get_parameters().size(), 1);
    EXPECT_EQ(std::get<std::string>(log_level.get_parameters()[0]), "debug");
    EXPECT_EQ(log_level.get_children().size(), 0);
}

TEST(ParserTests, ParseSingleDirectiveWithChildren)
{
    Parser parser("http { root /www; }");
    Config config("", Config::MAIN);
    parser.parse(config);

    auto http = *config.get_children()[0];

    EXPECT_EQ(http.get_type(), Type::HTTP);
    EXPECT_EQ(http.get_parameters().size(), 0);
    EXPECT_EQ(http.get_children().size(), 1);

    auto& root = *http.get_children()[0];
    EXPECT_EQ(root.get_type(), Type::ROOT);
    EXPECT_EQ(root.get_parameters().size(), 1);
    EXPECT_EQ(std::get<std::string>(root.get_parameters()[0]), "/www");
    EXPECT_EQ(root.get_children().size(), 0);
}

TEST(ParserTests, ParseSingleDirectiveWithMultipleChildren)
{
    Parser parser("http { root /www; autoindex on; }");
    Config config("", Config::MAIN);
    parser.parse(config);

    auto http = *config.get_children()[0];

    EXPECT_EQ(http.get_type(), Type::HTTP);
    EXPECT_EQ(http.get_parameters().size(), 0);
    EXPECT_EQ(http.get_children().size(), 2);

    auto& root = *http.get_children()[0];
    EXPECT_EQ(root.get_type(), Type::ROOT);
    EXPECT_EQ(root.get_parameters().size(), 1);
    EXPECT_EQ(std::get<std::string>(root.get_parameters()[0]), "/www");
    EXPECT_EQ(root.get_children().size(), 0);

    auto& autoindex = *http.get_children()[1];
    EXPECT_EQ(autoindex.get_type(), Type::AUTOINDEX);
    EXPECT_EQ(autoindex.get_parameters().size(), 1);
    EXPECT_EQ(std::get<bool>(autoindex.get_parameters()[0]), true);
    EXPECT_EQ(autoindex.get_children().size(), 0);
}

TEST(ParserTests, ParseMultipleDirectives)
{
    Parser parser("log_level debug; http { root /www; }");
    Config config("", Config::MAIN);
    parser.parse(config);

    EXPECT_EQ(config.get_type(), Type::MAIN);
    EXPECT_EQ(config.get_parameters().size(), 0);
    EXPECT_EQ(config.get_children().size(), 2);

    auto& log_level = *config.get_children()[0];
    EXPECT_EQ(log_level.get_type(), Type::LOG_LEVEL);
    EXPECT_EQ(log_level.get_parameters().size(), 1);
    EXPECT_EQ(std::get<std::string>(log_level.get_parameters()[0]), "debug");
    EXPECT_EQ(log_level.get_children().size(), 0);

    auto& http = *config.get_children()[1];
    EXPECT_EQ(http.get_type(), Type::HTTP);
    EXPECT_EQ(http.get_parameters().size(), 0);
    EXPECT_EQ(http.get_children().size(), 1);

    auto& root = *http.get_children()[0];
    EXPECT_EQ(root.get_type(), Type::ROOT);
    EXPECT_EQ(root.get_parameters().size(), 1);
    EXPECT_EQ(std::get<std::string>(root.get_parameters()[0]), "/www");
    EXPECT_EQ(root.get_children().size(), 0);
}

TEST(ParserTests, ParseNestedDirectives)
{
    Parser parser("http { server { listen 80; } }");
    Config config("", Config::MAIN);
    parser.parse(config);

    auto http = *config.get_children()[0];

    EXPECT_EQ(http.get_type(), Type::HTTP);
    EXPECT_EQ(http.get_parameters().size(), 0);
    EXPECT_EQ(http.get_children().size(), 1);

    auto& server = *http.get_children()[0];
    EXPECT_EQ(server.get_type(), Type::SERVER);
    EXPECT_EQ(server.get_parameters().size(), 0);
    EXPECT_EQ(server.get_children().size(), 1);

    auto& listen = *server.get_children()[0];
    EXPECT_EQ(listen.get_type(), Type::LISTEN);
    EXPECT_EQ(listen.get_parameters().size(), 1);
    EXPECT_EQ(std::get<int>(listen.get_parameters()[0]), 80);
    EXPECT_EQ(listen.get_children().size(), 0);
}

TEST(ParserTests, ParseSameDirectives)
{
    Parser parser("http { server { listen 80; listen 443; } }");
    Config config("", Config::MAIN);
    parser.parse(config);

    auto http = *config.get_children()[0];

    EXPECT_EQ(http.get_type(), Type::HTTP);
    EXPECT_EQ(http.get_parameters().size(), 0);
    EXPECT_EQ(http.get_children().size(), 1);

    auto& server = *http.get_children()[0];
    EXPECT_EQ(server.get_type(), Type::SERVER);
    EXPECT_EQ(server.get_parameters().size(), 0);
    EXPECT_EQ(server.get_children().size(), 2);

    auto& listen = *server.get_children()[0];
    EXPECT_EQ(listen.get_type(), Type::LISTEN);
    EXPECT_EQ(listen.get_parameters().size(), 1);
    EXPECT_EQ(std::get<int>(listen.get_parameters()[0]), 80);

    auto& listen2 = *server.get_children()[1];
    EXPECT_EQ(listen2.get_type(), Type::LISTEN);
    EXPECT_EQ(listen2.get_parameters().size(), 1);
    EXPECT_EQ(std::get<int>(listen2.get_parameters()[0]), 443);
}

////////////////////////////////////////
// Tests expected to fail
////////////////////////////////////////

// Missing bracket
TEST(ParserTests, ParseMissingBracket)
{
    Parser parser("http { server { listen 80; }");
    Config config("", Config::MAIN);

    EXPECT_THROW(parser.parse(config);, std::runtime_error);
}

// Unallowed directive tests
TEST(ParserTests, ParseUnallowedDirective)
{
    Config config("", Config::MAIN);

    Parser parser(" unallowed_directive; ");
    EXPECT_THROW(parser.parse(config), std::runtime_error);

    Parser parser2("http { unallowed_directive; }");
    EXPECT_THROW(parser.parse(config), std::runtime_error);

    Parser parser3("http { server { unallowed_directive; } }");
    EXPECT_THROW(parser3.parse(config), std::runtime_error);

    Parser parser4(" unallowed_directive { log_level debug; } ");
    EXPECT_THROW(parser4.parse(config), std::runtime_error);
}

// Test unique directive
TEST(ParserTests, ParseUniqueDirective)
{
    Config config("", Config::MAIN);

    Parser parser("http { log_level debug; log_level info; }");
    EXPECT_THROW(parser.parse(config), std::runtime_error);

    Parser parser2("http { server { log_level debug; log_level info; } }");
    EXPECT_THROW(parser2.parse(config), std::runtime_error);

    Parser parser3("log_level debug; log_level info;");
    EXPECT_THROW(parser3.parse(config), std::runtime_error);
}

// Test minimum number of parameters
TEST(ParserTests, ParseMinParams)
{
    Config config("", Config::MAIN);

    Parser parser("http { server { listen; } }");
    EXPECT_THROW(parser.parse(config), std::runtime_error);

    Parser parser2("http { server { listen 80; location; } }");
    EXPECT_THROW(parser2.parse(config), std::runtime_error);

    Parser parser3("http { server { listen 80; location /; } }");
    EXPECT_NO_THROW(parser3.parse(config));
}

// Test unique directive
TEST(ParserTests, ParseMaxParams)
{
    Config config("", Config::MAIN);

    Parser parser("http { server { index index.html; index hello.html; } }");
    EXPECT_THROW(parser.parse(config), std::runtime_error);

    Parser parser2("http { server { autoindex on; autoindex on; } }");
    EXPECT_THROW(parser2.parse(config), std::runtime_error);

    Parser parser3("http { server { location / { limit_except GET; limit_except POST; } } }");
    EXPECT_THROW(parser3.parse(config), std::runtime_error);
}

/*
TEST(ParserTests, ParseAllowedParameters)
{
    Parser parser("log_level invalid;");
    EXPECT_THROW(parser.parse(), std::runtime_error);

    Parser parser2("http { server { autoindex invalid; } }");
    EXPECT_THROW(parser2.parse(), std::runtime_error);

    Parser parser3("http { server { location / { limit_except invalid; } } }");
    EXPECT_THROW(parser3.parse(), std::runtime_error);
}
*/
