//
// Created by paco on 01/09/26.
//

#ifndef PACOCOMPILER_LEXER_H
#define PACOCOMPILER_LEXER_H

typedef enum {
    TOKEN_INVALID,
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,
    TOKEN_OPERATOR,
    //TOKEN_SIMPLE_OPERATOR,
    //TOKEN_COMPLEX_OPERATOR,
    TOKEN_NUMBER,
    TOKEN_STRING,
} TokenType;

static constexpr int ERROR_OFFSET = 1000;

typedef enum {
    LEX_SUCCESS = 0,
    ER_IDK      = ERROR_OFFSET,

    ER_STRING_LEFT_OPEN,      // ("abc)
    ER_STRING_ALREADY_CLOSED, // ("abc"de)

    ER_IDENTIFIER_LETTER_AFTER_DIGIT, // (foo123a)

    ERROR_COUNT
} LexerResult;

LexerResult parseLexeme(const char* input, TokenType* token_type);

#endif //PACOCOMPILER_LEXER_H
