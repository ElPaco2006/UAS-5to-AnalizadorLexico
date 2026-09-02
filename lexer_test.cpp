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

    TokenType type           = TOKEN_INVALID;
    const LexerResult result = parseLexeme(input, &type);

    ASSERT_EQ(type, expected_type);
    EXPECT_EQ(result, expected_result) << "Input: " << input << std::endl;
}

// Numbers
INSTANTIATE_TEST_SUITE_P(
    NumberInteger,
    LexerTest,
    testing::Values(
        LexerSample{"1", LEX_SUCCESS, TOKEN_NUMBER},
        LexerSample{"1234567890", LEX_SUCCESS, TOKEN_NUMBER},
        LexerSample{"-1", LEX_SUCCESS, TOKEN_NUMBER},
        LexerSample{"-1234567890", LEX_SUCCESS, TOKEN_NUMBER},

        LexerSample{"123_", ER_NUMBER_INVALID_CHARACTER, TOKEN_NUMBER},
        LexerSample{"-123_", ER_NUMBER_INVALID_CHARACTER, TOKEN_NUMBER}
    )
);

INSTANTIATE_TEST_SUITE_P(
    NumberReal,
    LexerTest,
    testing::Values(
        LexerSample{"1.0", LEX_SUCCESS, TOKEN_NUMBER},
        LexerSample{"1234.56789", LEX_SUCCESS, TOKEN_NUMBER},
        LexerSample{"-1.0", LEX_SUCCESS, TOKEN_NUMBER},
        LexerSample{"-1234.56789", LEX_SUCCESS, TOKEN_NUMBER},

        LexerSample{"1.0.0", ER_NUMBER_MORE_THAN_ONE_DECIMAL_POINT, TOKEN_NUMBER},
        LexerSample{"-1.0.0", ER_NUMBER_MORE_THAN_ONE_DECIMAL_POINT, TOKEN_NUMBER}
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

        LexerSample{"8.0.0e2", ER_NUMBER_MORE_THAN_ONE_DECIMAL_POINT, TOKEN_NUMBER},
        LexerSample{"8.0E++2", ER_NUMBER_MORE_THAN_ONE_EXPONENT_SIGN, TOKEN_NUMBER},
        LexerSample{"8.0E--2", ER_NUMBER_MORE_THAN_ONE_EXPONENT_SIGN, TOKEN_NUMBER},
        LexerSample{"8.0E-2.4", ER_NUMBER_DECIMAL_ON_EXPONENT, TOKEN_NUMBER}
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
        LexerSample{"<=", LEX_SUCCESS, TOKEN_OPERATOR},
        LexerSample{">=", LEX_SUCCESS, TOKEN_OPERATOR}

        // TODO: Add errors
    )
);

INSTANTIATE_TEST_SUITE_P(
    Words,
    LexerTest,
    testing::Values(
        LexerSample{"foo", LEX_SUCCESS, TOKEN_IDENTIFIER},
        LexerSample{"foo_bar", LEX_SUCCESS, TOKEN_IDENTIFIER},
        LexerSample{"foo123", LEX_SUCCESS, TOKEN_IDENTIFIER},
        LexerSample{"while_123", LEX_SUCCESS, TOKEN_IDENTIFIER},

        LexerSample{"if", LEX_SUCCESS, TOKEN_KEYWORD},
        LexerSample{"for", LEX_SUCCESS, TOKEN_KEYWORD},
        LexerSample{"while", LEX_SUCCESS, TOKEN_KEYWORD},

        LexerSample{"for_123_", ER_IDENTIFIER_LETTER_AFTER_DIGIT, TOKEN_IDENTIFIER}
        // TODO: Add errors
    )
);
