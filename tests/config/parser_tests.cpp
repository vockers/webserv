#include <gtest/gtest.h>

#include "config/Parser.hpp"

using namespace webserv::config;
using Type = Directive::Type;

TEST(ParserTests, ParseEmptyConfig)
{
    Parser parser("");

    auto config = parser.parse();

    EXPECT_EQ(config.get_type(), Directive::MAIN);
    EXPECT_EQ(config.get_parameters().size(), 0);
    EXPECT_EQ(config.get_children().size(), 0);
}

TEST(ParserTests, ParseSingleDirective)
{
    Parser parser("log_level debug;");

    auto config = *parser.parse().get_children()[0];

    EXPECT_EQ(config.get_type(), Directive::LOG_LEVEL);
    EXPECT_EQ(config.get_parameters().size(), 1);
    EXPECT_EQ(std::get<std::string>(config.get_parameters()[0]), "debug");
    EXPECT_EQ(config.get_children().size(), 0);
}

TEST(ParserTests, ParseSingleDirectiveWithChildren)
{
    Parser parser("http { root /www; }");

    auto config = *parser.parse().get_children()[0];

    EXPECT_EQ(config.get_type(), Type::HTTP);
    EXPECT_EQ(config.get_parameters().size(), 0);
    EXPECT_EQ(config.get_children().size(), 1);

    auto& child = *config.get_children()[0];
    EXPECT_EQ(child.get_type(), Type::ROOT);
    EXPECT_EQ(child.get_parameters().size(), 1);
    EXPECT_EQ(std::get<std::string>(child.get_parameters()[0]), "/www");
    EXPECT_EQ(child.get_children().size(), 0);
}

TEST(ParserTests, ParseSingleDirectiveWithMultipleChildren)
{
    Parser parser("http { root /www; autoindex on; }");

    auto config = *parser.parse().get_children()[0];

    EXPECT_EQ(config.get_type(), Type::HTTP);
    EXPECT_EQ(config.get_parameters().size(), 0);
    EXPECT_EQ(config.get_children().size(), 2);

    auto& child_1 = *config.get_children()[0];
    EXPECT_EQ(child_1.get_type(), Type::ROOT);
    EXPECT_EQ(child_1.get_parameters().size(), 1);
    EXPECT_EQ(std::get<std::string>(child_1.get_parameters()[0]), "/www");
    EXPECT_EQ(child_1.get_children().size(), 0);

    auto& child_2 = *config.get_children()[1];
    EXPECT_EQ(child_2.get_type(), Type::AUTOINDEX);
    EXPECT_EQ(child_2.get_parameters().size(), 1);
    EXPECT_EQ(std::get<bool>(child_2.get_parameters()[0]), true);
    EXPECT_EQ(child_2.get_children().size(), 0);
}

TEST(ParserTests, ParseMultipleDirectives)
{
    Parser parser("log_level debug; http { root /www; }");

    auto config = parser.parse();

    EXPECT_EQ(config.get_type(), Type::MAIN);
    EXPECT_EQ(config.get_parameters().size(), 0);
    EXPECT_EQ(config.get_children().size(), 2);

    auto& child1 = *config.get_children()[0];
    EXPECT_EQ(child1.get_type(), Type::LOG_LEVEL);
    EXPECT_EQ(child1.get_parameters().size(), 1);
    EXPECT_EQ(std::get<std::string>(child1.get_parameters()[0]), "debug");
    EXPECT_EQ(child1.get_children().size(), 0);

    auto& child2 = *config.get_children()[1];
    EXPECT_EQ(child2.get_type(), Type::HTTP);
    EXPECT_EQ(child2.get_parameters().size(), 0);
    EXPECT_EQ(child2.get_children().size(), 1);

    auto& child3 = *child2.get_children()[0];
    EXPECT_EQ(child3.get_type(), Type::ROOT);
    EXPECT_EQ(child3.get_parameters().size(), 1);
    EXPECT_EQ(std::get<std::string>(child3.get_parameters()[0]), "/www");
    EXPECT_EQ(child3.get_children().size(), 0);
}

TEST(ParserTests, ParseNestedDirectives)
{
    Parser parser("http { server { listen 80; } }");

    auto config = *parser.parse().get_children()[0];

    EXPECT_EQ(config.get_type(), Type::HTTP);
    EXPECT_EQ(config.get_parameters().size(), 0);
    EXPECT_EQ(config.get_children().size(), 1);

    auto& child1 = *config.get_children()[0];
    EXPECT_EQ(child1.get_type(), Type::SERVER);
    EXPECT_EQ(child1.get_parameters().size(), 0);
    EXPECT_EQ(child1.get_children().size(), 1);

    auto& child2 = *child1.get_children()[0];
    EXPECT_EQ(child2.get_type(), Type::LISTEN);
    EXPECT_EQ(child2.get_parameters().size(), 1);
    EXPECT_EQ(std::get<int>(child2.get_parameters()[0]), 80);
    EXPECT_EQ(child2.get_children().size(), 0);
}

TEST(ParserTests, ParseSameDirectives)
{
    Parser parser("http { server { listen 80; listen 443; } }");

    auto config = *parser.parse().get_children()[0];

    EXPECT_EQ(config.get_type(), Type::HTTP);
    EXPECT_EQ(config.get_parameters().size(), 0);
    EXPECT_EQ(config.get_children().size(), 1);

    auto& child1 = *config.get_children()[0];
    EXPECT_EQ(child1.get_type(), Type::SERVER);
    EXPECT_EQ(child1.get_parameters().size(), 0);
    EXPECT_EQ(child1.get_children().size(), 2);

    auto& child2 = *child1.get_children()[0];
    EXPECT_EQ(child2.get_type(), Type::LISTEN);
    EXPECT_EQ(child2.get_parameters().size(), 1);
    EXPECT_EQ(std::get<int>(child2.get_parameters()[0]), 80);

    auto& child3 = *child1.get_children()[1];
    EXPECT_EQ(child3.get_type(), Type::LISTEN);
    EXPECT_EQ(child3.get_parameters().size(), 1);
    EXPECT_EQ(std::get<int>(child3.get_parameters()[0]), 443);
}

////////////////////////////////////////
// Tests expected to fail
////////////////////////////////////////

// Missing bracket
TEST(ParserTests, ParseMissingBracket)
{
    Parser parser("http { server { listen 80; }");

    EXPECT_THROW(parser.parse(), std::runtime_error);
}

// Unallowed directive tests
TEST(ParserTests, ParseUnallowedDirective)
{
    Parser parser(" unallowed_directive; ");
    EXPECT_THROW(parser.parse(), std::runtime_error);

    Parser parser2("http { unallowed_directive; }");
    EXPECT_THROW(parser2.parse(), std::runtime_error);

    Parser parser3("http { server { unallowed_directive; } }");
    EXPECT_THROW(parser3.parse(), std::runtime_error);

    Parser parser4(" unallowed_directive { log_level debug; } ");
    EXPECT_THROW(parser4.parse(), std::runtime_error);
}

// Test unique directive
TEST(ParserTests, ParseUniqueDirective)
{
    Parser parser("http { log_level debug; log_level info; }");
    EXPECT_THROW(parser.parse(), std::runtime_error);

    Parser parser2("http { server { log_level debug; log_level info; } }");
    EXPECT_THROW(parser2.parse(), std::runtime_error);

    Parser parser3("log_level debug; log_level info;");
    EXPECT_THROW(parser3.parse(), std::runtime_error);
}

// Test minimum number of parameters
TEST(ParserTests, ParseMinParams)
{
    Parser parser("http { server { listen; } }");
    EXPECT_THROW(parser.parse(), std::runtime_error);

    Parser parser2("http { server { listen 80; location; } }");
    EXPECT_THROW(parser2.parse(), std::runtime_error);

    Parser parser3("http { server { listen 80; location /; } }");
    EXPECT_NO_THROW(parser3.parse());
}

// Test unique directive
TEST(ParserTests, ParseMaxParams)
{
    Parser parser("http { server { index index.html; index hello.html; } }");
    EXPECT_THROW(parser.parse(), std::runtime_error);

    Parser parser2("http { server { autoindex on; autoindex on; } }");
    EXPECT_THROW(parser2.parse(), std::runtime_error);

    Parser parser3("http { server { location / { limit_except GET; limit_except POST; } } }");
    EXPECT_THROW(parser3.parse(), std::runtime_error);
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
