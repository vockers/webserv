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

    auto config = parser.parse();

    EXPECT_EQ(config.get_name(), "log_level");
    EXPECT_EQ(config.get_parameters().size(), 1);
    EXPECT_EQ(config.get_parameters()[0], "debug");
    EXPECT_EQ(config.get_children().size(), 0);
}

TEST(ParserTests, ParseSingleDirectiveWithChildren)
{
    Parser parser("http { root /www; }");

    auto config = parser.parse();

    EXPECT_EQ(config.get_name(), "http");
    EXPECT_EQ(config.get_parameters().size(), 0);
    EXPECT_EQ(config.get_children().size(), 1);

    auto& child = config.get_children()[0];
    EXPECT_EQ(child.get_name(), "root");
    EXPECT_EQ(child.get_parameters().size(), 1);
    EXPECT_EQ(child.get_parameters()[0], "/www");
    EXPECT_EQ(child.get_children().size(), 0);
}

/*TEST(ParserTests, ParseMultipleDirectives)*/
/*{*/
/*    Parser parser("log_level debug; http { root /www; }");*/
/**/
/*    auto config = parser.parse();*/
/**/
/*    EXPECT_EQ(config.get_name(), "");*/
/*    EXPECT_EQ(config.get_parameters().size(), 0);*/
/*    EXPECT_EQ(config.get_children().size(), 2);*/
/**/
/*    auto& child1 = config.get_children()[0];*/
/*    EXPECT_EQ(child1.get_name(), "log_level");*/
/*    EXPECT_EQ(child1.get_parameters().size(), 1);*/
/*    EXPECT_EQ(child1.get_parameters()[0], "debug");*/
/*    EXPECT_EQ(child1.get_children().size(), 0);*/
/**/
/*    auto& child2 = config.get_children()[1];*/
/*    EXPECT_EQ(child2.get_name(), "http");*/
/*    EXPECT_EQ(child2.get_parameters().size(), 0);*/
/*    EXPECT_EQ(child2.get_children().size(), 1);*/
/**/
/*    auto& child3 = child2.get_children()[0];*/
/*    EXPECT_EQ(child3.get_name(), "root");*/
/*    EXPECT_EQ(child3.get_parameters().size(), 1);*/
/*    EXPECT_EQ(child3.get_parameters()[0], "/www");*/
/*    EXPECT_EQ(child3.get_children().size(), 0);*/
/*}*/
