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
    EXPECT_EQ(token.type, Lexer::Token::Type::WORD);
    EXPECT_EQ(token.value, "word");

    token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::NONE);
}

TEST(LexerTests, SingleWordWithWhitespace)
{
    Lexer lexer("  word  ");
    auto token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::WORD);
    EXPECT_EQ(token.value, "word");
}

TEST(LexerTests, SingleWordWithWhitespaceAndSemicolon)
{
    Lexer lexer("  word;  ");
    auto token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::WORD);
    EXPECT_EQ(token.value, "word");

    token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::RULE_END);
}

TEST(LexerTests, SingleWordWithWhitespaceAndBraces)
{
    Lexer lexer("  word {  ");
    auto token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::WORD);
    EXPECT_EQ(token.value, "word");

    token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::BLOCK_START);
}

TEST(LexerTests, SingleWordWithWhitespaceAndBracesAndSemicolon)
{
    Lexer lexer("  word { };  ");
    auto token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::WORD);
    EXPECT_EQ(token.value, "word");

    token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::BLOCK_START);

    token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::BLOCK_END);

    token = lexer.next_token();
    EXPECT_EQ(token.type, Lexer::Token::Type::RULE_END);
}

