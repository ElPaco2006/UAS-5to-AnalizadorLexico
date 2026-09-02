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

bool parseLexeme(const char* input, TokenType* token_type);

#endif //PACOCOMPILER_LEXER_H
