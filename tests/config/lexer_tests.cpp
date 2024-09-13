#include <gtest/gtest.h>

#include "config/Lexer.hpp"

TEST(LexerTests, EmptyInput)
{
    config::Lexer lexer("");
    auto token = lexer.next_token();
    EXPECT_EQ(token.type, config::Lexer::Token::Type::NONE);
}

TEST(LexerTests, SingleWord)
{
    config::Lexer lexer("word");
    auto token = lexer.next_token();
    EXPECT_EQ(token.type, config::Lexer::Token::Type::WORD);
    EXPECT_EQ(token.value, "word");

    token = lexer.next_token();
    EXPECT_EQ(token.type, config::Lexer::Token::Type::NONE);
}

TEST(LexerTests, SingleWordWithWhitespace)
{
    config::Lexer lexer("  word  ");
    auto token = lexer.next_token();
    EXPECT_EQ(token.type, config::Lexer::Token::Type::WORD);
    EXPECT_EQ(token.value, "word");
}

TEST(LexerTests, SingleWordWithWhitespaceAndSemicolon)
{
    config::Lexer lexer("  word;  ");
    auto token = lexer.next_token();
    EXPECT_EQ(token.type, config::Lexer::Token::Type::WORD);
    EXPECT_EQ(token.value, "word");

    token = lexer.next_token();
    EXPECT_EQ(token.type, config::Lexer::Token::Type::RULE_END);
}

TEST(LexerTests, SingleWordWithWhitespaceAndBraces)
{
    config::Lexer lexer("  word {  ");
    auto token = lexer.next_token();
    EXPECT_EQ(token.type, config::Lexer::Token::Type::WORD);
    EXPECT_EQ(token.value, "word");

    token = lexer.next_token();
    EXPECT_EQ(token.type, config::Lexer::Token::Type::BLOCK_START);
}
