#include <gtest/gtest.h>

#include "config/Config.hpp"

using namespace webserv::config;

TEST(ConfigTests, ConfigTest)
{
    Config config("tests/conf/test.conf");

    const Directive& main = config.get_main_directive();

    EXPECT_EQ(main.get_name(), "");
    EXPECT_EQ(main.get_children().size(), 1);
    
    auto& child = main.get_children()[0];
    EXPECT_EQ(child.get_name(), "http");
    EXPECT_EQ(child.get_children().size(), 1);

    auto& child2 = child.get_children()[0];
    EXPECT_EQ(child2.get_name(), "server");
    EXPECT_EQ(child2.get_children().size(), 1);

    auto& child3 = child2.get_children()[0];
    EXPECT_EQ(child3.get_name(), "listen");
    EXPECT_EQ(child3.get_parameters().size(), 1);
    EXPECT_EQ(child3.get_parameters()[0], "8080");
}
