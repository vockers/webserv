#include <gtest/gtest.h>

#include "config/Config.hpp"

using namespace webserv::config;
using Type = Directive::Type;

TEST(ConfigTests, ConfigTest)
{
    Config config("tests/conf/test.conf");

    const Directive& main = config.get_main_directive();

    EXPECT_EQ(main.get_type(), Type::MAIN);
    EXPECT_EQ(main.get_children().size(), 2);

    EXPECT_EQ(std::get<std::string>(main.get_parameters(Type::LOG_LEVEL)[0]), "crit");
    
    auto& child = main.get_children()[1];
    EXPECT_EQ(child.get_type(), Type::HTTP);
    EXPECT_EQ(child.get_children().size(), 1);

    auto& child2 = child.get_children()[0];
    EXPECT_EQ(child2.get_type(), Type::SERVER);
    EXPECT_EQ(child2.get_children().size(), 2);

    EXPECT_EQ(std::get<std::string>(child2.get_parameters(Type::SERVER_NAME)[0]), "localhost");
    EXPECT_EQ(std::get<std::string>(child2.get_parameters(Type::LISTEN)[0]), "8080");
}
