//
// Created by paco on 01/09/26.
//

#include <gtest/gtest.h>

extern "C" {
#include "lexer.h"
}

struct LexerTestSample {
    const char* input;
    LexerResult expected_result;
    TokenType expected_type;
};

class LexerTest : public testing::TestWithParam<LexerTestSample> {};

TEST_P(LexerTest, TestSample) {
    const auto& [input, expected_result, expected_type] = GetParam();

    TokenType type;
    const LexerResult result = parseLexeme(input, &type);

    ASSERT_EQ(type, expected_type);
    EXPECT_EQ(result, expected_result);
}

// Numbers
INSTANTIATE_TEST_SUITE_P(
    NumberInteger,
    LexerTest,
    testing::Values(
        LexerTestSample{"1", LEX_SUCCESS, TOKEN_NUMBER},
        LexerTestSample{"1234567890", LEX_SUCCESS, TOKEN_NUMBER},
        LexerTestSample{"123_", ER_IDK, TOKEN_NUMBER},
        LexerTestSample{"-1", LEX_SUCCESS, TOKEN_NUMBER},
        LexerTestSample{"-1234567890", LEX_SUCCESS, TOKEN_NUMBER},
        LexerTestSample{"-123_", ER_IDK, TOKEN_NUMBER}
    )
);

INSTANTIATE_TEST_SUITE_P(
    NumberReal,
    LexerTest,
    testing::Values(
        LexerTestSample{"1.0", LEX_SUCCESS, TOKEN_NUMBER},
        LexerTestSample{"1234.56789", LEX_SUCCESS, TOKEN_NUMBER},
        LexerTestSample{"1.0.0", ER_IDK, TOKEN_NUMBER},
        LexerTestSample{"-1.0", LEX_SUCCESS, TOKEN_NUMBER},
        LexerTestSample{"-1234.56789", LEX_SUCCESS, TOKEN_NUMBER},
        LexerTestSample{"-1.0.0", ER_IDK, TOKEN_NUMBER}
    )
);

INSTANTIATE_TEST_SUITE_P(
    NumberScientific,
    LexerTest,
    testing::Values(
        LexerTestSample{"8E2", LEX_SUCCESS, TOKEN_NUMBER},
        LexerTestSample{"8e2", LEX_SUCCESS, TOKEN_NUMBER},
        LexerTestSample{"-8E2", LEX_SUCCESS, TOKEN_NUMBER},
        LexerTestSample{"8.0E2", LEX_SUCCESS, TOKEN_NUMBER},
        LexerTestSample{"8.0E+2", LEX_SUCCESS, TOKEN_NUMBER},
        LexerTestSample{"8.0E-2", LEX_SUCCESS, TOKEN_NUMBER},

        LexerTestSample{"8.0.0e2", ER_IDK, TOKEN_NUMBER},
        LexerTestSample{"8.0E++2", ER_IDK, TOKEN_NUMBER},
        LexerTestSample{"8.0E--2", ER_IDK, TOKEN_NUMBER}
    )
);


// Words

// Operators
