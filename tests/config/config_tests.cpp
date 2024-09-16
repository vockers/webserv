#include <gtest/gtest.h>

#include "config/Config.hpp"

using namespace webserv::config;
using Type = Directive::Type;

TEST(ConfigTests, ConfigTest)
{
    Config config("tests/conf/test.conf");

    const Directive& main = config.get_main_directive();

    EXPECT_EQ(main.get_type(), Type::MAIN);
    EXPECT_EQ(main.get_children().size(), 1);
    
    auto& child = main.get_children()[0];
    EXPECT_EQ(child.get_type(), Type::HTTP);
    EXPECT_EQ(child.get_children().size(), 1);

    auto& child2 = child.get_children()[0];
    EXPECT_EQ(child2.get_type(), Type::SERVER);
    EXPECT_EQ(child2.get_children().size(), 1);

    auto& child3 = child2.get_children()[0];
    EXPECT_EQ(child3.get_type(), Type::LISTEN);
    EXPECT_EQ(child3.get_parameters().size(), 1);
    EXPECT_EQ(std::get<std::string>(child3.get_parameters()[0]), "8080");
}
