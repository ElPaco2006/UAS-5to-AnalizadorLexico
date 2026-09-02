//
// Created by paco on 01/09/26.
//

#include <gtest/gtest.h>

extern "C" {
#include "lexer.h"
}

struct LexerSample {
    const char* input;
    LexerResult expected_result;
    TokenType expected_type;
};

class LexerTest : public testing::TestWithParam<LexerSample> {};

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
        LexerSample{"1", LEX_SUCCESS, TOKEN_NUMBER},
        LexerSample{"1234567890", LEX_SUCCESS, TOKEN_NUMBER},
        LexerSample{"123_", ER_IDK, TOKEN_NUMBER},
        LexerSample{"-1", LEX_SUCCESS, TOKEN_NUMBER},
        LexerSample{"-1234567890", LEX_SUCCESS, TOKEN_NUMBER},
        LexerSample{"-123_", ER_IDK, TOKEN_NUMBER}
    )
);

INSTANTIATE_TEST_SUITE_P(
    NumberReal,
    LexerTest,
    testing::Values(
        LexerSample{"1.0", LEX_SUCCESS, TOKEN_NUMBER},
        LexerSample{"1234.56789", LEX_SUCCESS, TOKEN_NUMBER},
        LexerSample{"1.0.0", ER_IDK, TOKEN_NUMBER},
        LexerSample{"-1.0", LEX_SUCCESS, TOKEN_NUMBER},
        LexerSample{"-1234.56789", LEX_SUCCESS, TOKEN_NUMBER},
        LexerSample{"-1.0.0", ER_IDK, TOKEN_NUMBER}
    )
);

INSTANTIATE_TEST_SUITE_P(
    NumberScientific,
    LexerTest,
    testing::Values(
        LexerSample{"8E2", LEX_SUCCESS, TOKEN_NUMBER},
        LexerSample{"8e2", LEX_SUCCESS, TOKEN_NUMBER},
        LexerSample{"-8E2", LEX_SUCCESS, TOKEN_NUMBER},
        LexerSample{"8.0E2", LEX_SUCCESS, TOKEN_NUMBER},
        LexerSample{"8.0E+2", LEX_SUCCESS, TOKEN_NUMBER},
        LexerSample{"8.0E-2", LEX_SUCCESS, TOKEN_NUMBER},

        LexerSample{"8.0.0e2", ER_IDK, TOKEN_NUMBER},
        LexerSample{"8.0E++2", ER_IDK, TOKEN_NUMBER},
        LexerSample{"8.0E--2", ER_IDK, TOKEN_NUMBER}
    )
);

// Words
INSTANTIATE_TEST_SUITE_P(
    Strings,
    LexerTest,
    testing::Values(
        LexerSample{"\"Hello, World!\"", LEX_SUCCESS, TOKEN_STRING},
        LexerSample{"\"@`~^[]*    asdd \"", LEX_SUCCESS, TOKEN_STRING},
        LexerSample{"\"\"", LEX_SUCCESS, TOKEN_STRING},

        LexerSample{"\"Hello, World!", ER_STRING_LEFT_OPEN, TOKEN_STRING},
        LexerSample{"\"Hello, World!\"   abc", ER_STRING_ALREADY_CLOSED, TOKEN_STRING}
    )
);

// Operators
INSTANTIATE_TEST_SUITE_P(
    Operators,
    LexerTest,
    testing::Values(
        LexerSample{"=", LEX_SUCCESS, TOKEN_OPERATOR},
        LexerSample{"+", LEX_SUCCESS, TOKEN_OPERATOR},
        LexerSample{"-", LEX_SUCCESS, TOKEN_OPERATOR},
        LexerSample{"*", LEX_SUCCESS, TOKEN_OPERATOR},
        LexerSample{"/", LEX_SUCCESS, TOKEN_OPERATOR},
        LexerSample{"%", LEX_SUCCESS, TOKEN_OPERATOR},
        LexerSample{"!", LEX_SUCCESS, TOKEN_OPERATOR},
        LexerSample{"<", LEX_SUCCESS, TOKEN_OPERATOR},
        LexerSample{">", LEX_SUCCESS, TOKEN_OPERATOR},

        LexerSample{"++", LEX_SUCCESS, TOKEN_OPERATOR},
        LexerSample{"--", LEX_SUCCESS, TOKEN_OPERATOR},

        LexerSample{"==", LEX_SUCCESS, TOKEN_OPERATOR},
        LexerSample{"+=", LEX_SUCCESS, TOKEN_OPERATOR},
        LexerSample{"-=", LEX_SUCCESS, TOKEN_OPERATOR},
        LexerSample{"*=", LEX_SUCCESS, TOKEN_OPERATOR},
        LexerSample{"/=", LEX_SUCCESS, TOKEN_OPERATOR},
        LexerSample{"%=", LEX_SUCCESS, TOKEN_OPERATOR},
        LexerSample{"!=", LEX_SUCCESS, TOKEN_OPERATOR},
        LexerSample{"<=", LEX_SUCCESS, TOKEN_OPERATOR},
        LexerSample{">=", LEX_SUCCESS, TOKEN_OPERATOR}

        // TODO: Add errors
    )
);
