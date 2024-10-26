#include <gtest/gtest.h>

#include "config/Lexer.hpp"

using namespace webserv::config;

TEST(LexerTests, EmptyInput)
{
    Lexer lexer("");
    auto token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::NONE);
}

TEST(LexerTests, SingleWord)
{
    Lexer lexer("word");
    auto token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::STRING);
    EXPECT_EQ(token.get<std::string>(), "word");

    token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::NONE);
}

TEST(LexerTests, SingleWordWithWhitespace)
{
    Lexer lexer("  word  ");
    auto token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::STRING);
    EXPECT_EQ(token.get<std::string>(), "word");

	token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::NONE);
}

TEST(LexerTests, SingleWordWithWhitespaceAndSemicolon)
{
    Lexer lexer("  word;  ");
    auto token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::STRING);
    EXPECT_EQ(token.get<std::string>(), "word");

    token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::END);
}

TEST(LexerTests, SingleWordWithWhitespaceAndBraces)
{
    Lexer lexer("  word {  ");
    auto token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::STRING);
    EXPECT_EQ(token.get<std::string>(), "word");

    token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::BLOCK_START);
}

TEST(LexerTests, SingleWordWithWhitespaceAndBracesAndSemicolon)
{
    Lexer lexer("  word { };  ");
    auto token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::STRING);
    EXPECT_EQ(token.get<std::string>(), "word");

    token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::BLOCK_START);

    token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::BLOCK_END);

    token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::END);
}

TEST(LexerTests, StringNumberBool)
{
    Lexer lexer("string 123 on off");

    auto token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::STRING);
    EXPECT_EQ(token.get<std::string>(), "string");

    token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::NUMBER);
    EXPECT_EQ(token.get<int>(), 123);

    token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::BOOL);
    EXPECT_EQ(token.get<bool>(), true);

    token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::BOOL);
    EXPECT_EQ(token.get<bool>(), false);
}

TEST(LexerTests, WordComment)
{
    Lexer lexer("word# comment");
    auto token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::STRING);
    EXPECT_EQ(token.get<std::string>(), "word");

    token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::NONE);
}

TEST(LexerTests, WordSpaceComment)
{
    Lexer lexer("word # comment");
    auto token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::STRING);
    EXPECT_EQ(token.get<std::string>(), "word");

    token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::NONE);
}

TEST(LexerTests, Comment)
{
    Lexer lexer("# comment");
    auto token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::NONE);
}

TEST(LexerTests, SpaceComment)
{
    Lexer lexer("     # comment");
    auto token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::NONE);
}

TEST(LexerTests, CommentWordSpaceBlockStartEnd)
{
    Lexer lexer("     # comment\nword { # comment\n};");
	auto token = lexer.next_token();
	EXPECT_EQ(token.type, Lexer::Token::Type::STRING);
	EXPECT_EQ(token.get<std::string>(), "word");

	token = lexer.next_token();
	EXPECT_EQ(token.type, Lexer::Token::Type::BLOCK_START);

	token = lexer.next_token();
	EXPECT_EQ(token.type, Lexer::Token::Type::BLOCK_END);

	token = lexer.next_token();
	EXPECT_EQ(token.type, Lexer::Token::Type::END);
}
