#include <gtest/gtest.h>

#include "config/Config.hpp"

using namespace webserv::config;
using Type = Directive::Type;

TEST(ConfigTests, ConfigTest)
{
    Config config("tests/conf/test.conf");

    EXPECT_EQ(config.get<std::string>(Type::LOG_LEVEL, 0), "crit");

    const Directive& main = config;

    EXPECT_EQ(main.get_type(), Type::MAIN);

    EXPECT_EQ(main.get<std::string>(Type::LOG_LEVEL, 0), "crit");
    
    auto& http = *main.get_children()[1];
    EXPECT_EQ(http.get_type(), Type::HTTP);

    auto& server = *http.get_children()[0];
    EXPECT_EQ(server.get_type(), Type::SERVER);

    EXPECT_EQ(server.get<std::string>(Type::SERVER_NAME, 0), "localhost");
    EXPECT_EQ(server.get<int>(Type::LISTEN, 0), 8080);
    EXPECT_EQ(server.get<std::string>(Type::ROOT, 0), "/www");
    EXPECT_EQ(server.get<int>(Type::ERROR_PAGE, 0), 404);
    EXPECT_EQ(server.get<std::string>(Type::ERROR_PAGE, 1), "/404.html");
}

TEST(ConfigTests, DefaultValues)
{
    Config config("tests/conf/test.conf");

    const Directive& main = config;

    EXPECT_EQ(main.get<int>(Type::CLIENT_MAX_BODY_SIZE, 0), 1048576);
    EXPECT_EQ(main.get<bool>(Type::AUTOINDEX, 0), false);
    EXPECT_EQ(main.get<std::string>(Type::INDEX, 0), "index.html");

    auto& http = *main.get_children()[1];
    EXPECT_EQ(http.get<int>(Type::CLIENT_MAX_BODY_SIZE, 0), 1048576);
    EXPECT_EQ(http.get<bool>(Type::AUTOINDEX, 0), false);
    EXPECT_EQ(http.get<std::string>(Type::INDEX, 0), "index.html");

    auto& server = *http.get_children()[0];
    EXPECT_EQ(server.get_parent(), &http);
    EXPECT_EQ(server.get<int>(Type::CLIENT_MAX_BODY_SIZE, 0), 1048576);
    EXPECT_EQ(server.get<bool>(Type::AUTOINDEX, 0), false);
    EXPECT_EQ(server.get<std::string>(Type::INDEX, 0), "index.html");
}

TEST(ConfigTests, ErrorHandling)
{
    EXPECT_THROW(Config("tests/conf/invalid.conf"), std::runtime_error);

    Config config("tests/conf/test.conf");

    const Directive& main = config;

    auto& http = *main.get_children()[1];
    EXPECT_THROW(http.get<std::string>(Type::ERROR_PAGE, 0), std::runtime_error);

    auto& server = *http.get_children()[0];
    EXPECT_THROW(server.get<std::string>(Type::SERVER_NAME, 1), std::runtime_error);
    EXPECT_THROW(server.get<int>(Type::LISTEN, 1), std::runtime_error);
    EXPECT_THROW(server.get<std::string>(Type::ROOT, 1), std::runtime_error);
}
