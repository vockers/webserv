#include <gtest/gtest.h>

#include "config/Config.hpp"

using namespace webserv::config;
using Type = Config::Type;

TEST(ConfigTests, ConfigTest)
{
    Config config("tests/conf/test.conf");

    EXPECT_EQ(config.log_level(), "crit");

    const Config& main = config;

    EXPECT_EQ(main.get_type(), Type::MAIN);

    EXPECT_EQ(main.log_level(), "crit");

    auto& http = *main.get_children()[1];
    EXPECT_EQ(http.get_type(), Type::HTTP);

    auto& server = *http.get_children()[0];
    EXPECT_EQ(server.get_type(), Type::SERVER);

    EXPECT_EQ(server.server_name(), "localhost");
    EXPECT_EQ(server.listen(), 8080);
    EXPECT_EQ(server.error_page(404), "/404.html");

    auto& location = server.location("/l");
    EXPECT_EQ(location.root(), "/www");

    auto& error_location = server.location("/e/404.html");
    EXPECT_EQ(error_location.root(), "/www/errors");
}

TEST(ConfigTests, DefaultValues)
{
    Config config("tests/conf/test.conf");

    const Config& main = config;

    EXPECT_EQ(main.client_max_body_size(), 1048576);
    EXPECT_EQ(main.autoindex(), false);
    EXPECT_EQ(main.index(), "index.html");

    auto& http = *main.get_children()[1];
    EXPECT_EQ(http.client_max_body_size(), 1048576);
    EXPECT_EQ(http.autoindex(), false);
    EXPECT_EQ(http.index(), "index.html");

    auto& server = *http.get_children()[0];
    EXPECT_EQ(server.get_parent(), &http);
    EXPECT_EQ(server.client_max_body_size(), 1048576);
    EXPECT_EQ(server.autoindex(), false);
    EXPECT_EQ(server.index(), "index.html");
}

TEST(ConfigTests, ErrorHandling)
{
    EXPECT_THROW(Config("tests/conf/invalid.conf"), std::runtime_error);

    Config config("tests/conf/test.conf");

    const Config& main = config;

    auto& http = *main.get_children()[1];
    EXPECT_THROW(http.value<std::string>(Type::ERROR_PAGE, 0), std::runtime_error);

    auto& server = *http.get_children()[0];
    EXPECT_THROW(server.value<std::string>(Type::SERVER_NAME, 1), std::runtime_error);
    EXPECT_THROW(server.value<int>(Type::LISTEN, 1), std::runtime_error);
    EXPECT_THROW(server.value<std::string>(Type::ROOT, 1), std::runtime_error);
}
