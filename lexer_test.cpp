//
// Created by paco on 01/09/26.
//

#include <gtest/gtest.h>
#include "lexer.h"

TEST(HelloTest, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}

// Numbers
TEST(LexerNumbers, Integer) {
    const char* samples[] = {
        "1",
        "-1",
        "1234",
        "9",
        "010",
        "01",
    };
    constexpr int samples_count = sizeof(samples) / sizeof(samples[0]);

    for (int i = 0; i < samples_count; i++) {
        TokenType type;
        bool is_valid = parseLexeme(samples[i], &type);

        ASSERT_EQ(type, TOKEN_NUMBER);
        EXPECT_TRUE(is_valid);
    }
}

TEST(LexerNumbers, Real) {
    const char* samples[] = {
        "1.0",
        "-1.0",
        "1.123",
        "100.1000000000",
    };
    constexpr int samples_count = sizeof(samples) / sizeof(samples[0]);

    for (int i = 0; i < samples_count; i++) {
        TokenType type;
        bool is_valid = parseLexeme(samples[i], &type);

        ASSERT_EQ(type, TOKEN_NUMBER);
        EXPECT_TRUE(is_valid);
    }
}

TEST(LexerNumbers, ScientificNotation) {
    const char* samples[] = {
        "8E2",
        "8e2",
        "-8.0E2",
        "8.0E+2",
        "8.0E-2",
    };
    constexpr int samples_count = sizeof(samples) / sizeof(samples[0]);

    for (int i = 0; i < samples_count; i++) {
        TokenType type;
        bool is_valid = parseLexeme(samples[i], &type);

        ASSERT_EQ(type, TOKEN_NUMBER);
        EXPECT_TRUE(is_valid);
    }
}

// Strings

// Words

// Operators
