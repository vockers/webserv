#include <gtest/gtest.h>

#include "config/Parser.hpp"

using namespace webserv::config;

TEST(ParserTests, ParseEmptyConfig)
{
    Parser parser("");

    auto config = parser.parse();

    EXPECT_EQ(config.get_name(), "");
    EXPECT_EQ(config.get_parameters().size(), 0);
    EXPECT_EQ(config.get_children().size(), 0);
}

TEST(ParserTests, ParseSingleDirective)
{
    Parser parser("log_level debug;");

    auto config = parser.parse().get_children()[0];

    EXPECT_EQ(config.get_name(), "log_level");
    EXPECT_EQ(config.get_parameters().size(), 1);
    EXPECT_EQ(config.get_parameters()[0], "debug");
    EXPECT_EQ(config.get_children().size(), 0);
}

TEST(ParserTests, ParseSingleDirectiveWithChildren)
{
    Parser parser("http { root /www; }");

    auto config = parser.parse().get_children()[0];

    EXPECT_EQ(config.get_name(), "http");
    EXPECT_EQ(config.get_parameters().size(), 0);
    EXPECT_EQ(config.get_children().size(), 1);

    auto& child = config.get_children()[0];
    EXPECT_EQ(child.get_name(), "root");
    EXPECT_EQ(child.get_parameters().size(), 1);
    EXPECT_EQ(child.get_parameters()[0], "/www");
    EXPECT_EQ(child.get_children().size(), 0);
}

TEST(ParserTests, ParseSingleDirectiveWithMultipleChildren)
{
    Parser parser("http { root /www; autoindex on; }");

    auto config = parser.parse().get_children()[0];

    EXPECT_EQ(config.get_name(), "http");
    EXPECT_EQ(config.get_parameters().size(), 0);
    EXPECT_EQ(config.get_children().size(), 2);

    auto& child_1 = config.get_children()[0];
    EXPECT_EQ(child_1.get_name(), "root");
    EXPECT_EQ(child_1.get_parameters().size(), 1);
    EXPECT_EQ(child_1.get_parameters()[0], "/www");
    EXPECT_EQ(child_1.get_children().size(), 0);

    auto& child_2 = config.get_children()[1];
    EXPECT_EQ(child_2.get_name(), "autoindex");
    EXPECT_EQ(child_2.get_parameters().size(), 1);
    EXPECT_EQ(child_2.get_parameters()[0], "on");
    EXPECT_EQ(child_2.get_children().size(), 0);
}

TEST(ParserTests, ParseMultipleDirectives)
{
    Parser parser("log_level debug; http { root /www; }");

    auto config = parser.parse();

    EXPECT_EQ(config.get_name(), "");
    EXPECT_EQ(config.get_parameters().size(), 0);
    EXPECT_EQ(config.get_children().size(), 2);

    auto& child1 = config.get_children()[0];
    EXPECT_EQ(child1.get_name(), "log_level");
    EXPECT_EQ(child1.get_parameters().size(), 1);
    EXPECT_EQ(child1.get_parameters()[0], "debug");
    EXPECT_EQ(child1.get_children().size(), 0);

    auto& child2 = config.get_children()[1];
    EXPECT_EQ(child2.get_name(), "http");
    EXPECT_EQ(child2.get_parameters().size(), 0);
    EXPECT_EQ(child2.get_children().size(), 1);

    auto& child3 = child2.get_children()[0];
    EXPECT_EQ(child3.get_name(), "root");
    EXPECT_EQ(child3.get_parameters().size(), 1);
    EXPECT_EQ(child3.get_parameters()[0], "/www");
    EXPECT_EQ(child3.get_children().size(), 0);
}

TEST(ParserTests, ParseNestedDirectives)
{
    Parser parser("http { server { listen 80; } }");

    auto config = parser.parse().get_children()[0];

    EXPECT_EQ(config.get_name(), "http");
    EXPECT_EQ(config.get_parameters().size(), 0);
    EXPECT_EQ(config.get_children().size(), 1);

    auto& child1 = config.get_children()[0];
    EXPECT_EQ(child1.get_name(), "server");
    EXPECT_EQ(child1.get_parameters().size(), 0);
    EXPECT_EQ(child1.get_children().size(), 1);

    auto& child2 = child1.get_children()[0];
    EXPECT_EQ(child2.get_name(), "listen");
    EXPECT_EQ(child2.get_parameters().size(), 1);
    EXPECT_EQ(child2.get_parameters()[0], "80");
    EXPECT_EQ(child2.get_children().size(), 0);
}

// Tests expected to fail
TEST(ParserTests, ParseInvalidConfig)
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
