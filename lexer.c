//
// Created by paco on 01/09/26.
//

#include <string.h>
#include "lexer.h"

#include <stdio.h>

typedef enum {
    EOL, // \0

    LETTER,   // A-Z, a-z, _
    EXPONENT, // E, e

    DIGIT, // 0-9

    OPERATOR,
    PLUS,
    MINUS,
    EQUALS,

    QUOTES,
    DECIMAL_POINT,
    INVALID,

    SYMBOL_COUNT
} SymbolType;

typedef enum {
    ST_NULL  = 0,
    ST_START = 1,

    ST_NUMBER                 = 2,
    ST_NUMBER_POINT           = 3,
    ST_NUMBER_DECIMALS        = 4,
    ST_NUMBER_EXPONENT        = 5,
    ST_NUMBER_EXPONENT_SIGN   = 6,
    ST_NUMBER_EXPONENT_AMOUNT = 7,

    ST_STRING_OPEN   = 9,
    ST_STRING_CLOSED = 10,

    ST_OPERATOR_PLUS      = 11,
    ST_OPERATOR_MINUS     = 12,
    ST_OPERATOR_OTHER     = 13,
    ST_OPERATOR_INCREMENT = 14,
    ST_OPERATOR_DECREMENT = 15,
    ST_OPERATOR_COMPLEX   = 16,

    ST_IDENTIFIER              = 17,
    ST_IDENTIFIER_WITH_NUMBERS = 18,

    STATE_COUNT,
} LexerState;

static int lexer_matrix[STATE_COUNT][SYMBOL_COUNT] = {
    [ST_NULL]  = {0},
    [ST_START] = {
        [EOL]           = ER_EMPTY,
        [LETTER]        = ST_IDENTIFIER,
        [EXPONENT]      = ST_IDENTIFIER,
        [DIGIT]         = ST_NUMBER,
        [OPERATOR]      = ST_OPERATOR_OTHER,
        [PLUS]          = ST_OPERATOR_PLUS,
        [MINUS]         = ST_OPERATOR_MINUS,
        [EQUALS]        = ST_OPERATOR_OTHER,
        [QUOTES]        = ST_STRING_OPEN,
        [DECIMAL_POINT] = ER_NUMBER_MISSING_INTEGER_PART,
        [INVALID]       = ER_INVALID_CHARACTER,
    },

    [ST_NUMBER] = {
        [EOL]           = ST_NULL,
        [LETTER]        = ER_NUMBER_INVALID_CHARACTER,
        [EXPONENT]      = ST_NUMBER_EXPONENT,
        [DIGIT]         = ST_NUMBER,
        [OPERATOR]      = ER_NUMBER_INVALID_CHARACTER,
        [PLUS]          = ER_NUMBER_INVALID_CHARACTER,
        [MINUS]         = ER_NUMBER_INVALID_CHARACTER,
        [EQUALS]        = ER_NUMBER_INVALID_CHARACTER,
        [QUOTES]        = ER_NUMBER_INVALID_CHARACTER,
        [DECIMAL_POINT] = ST_NUMBER_POINT,
        [INVALID]       = ER_NUMBER_INVALID_CHARACTER,
    },
    [ST_NUMBER_POINT] = {
        [EOL]           = ER_NUMBER_MISSING_DECIMAL_PART,
        [LETTER]        = ER_NUMBER_INVALID_CHARACTER,
        [EXPONENT]      = ER_NUMBER_MISSING_DECIMAL_PART,
        [DIGIT]         = ST_NUMBER_DECIMALS,
        [OPERATOR]      = ER_NUMBER_INVALID_CHARACTER,
        [PLUS]          = ER_NUMBER_INVALID_CHARACTER,
        [MINUS]         = ER_NUMBER_INVALID_CHARACTER,
        [EQUALS]        = ER_NUMBER_INVALID_CHARACTER,
        [QUOTES]        = ER_NUMBER_INVALID_CHARACTER,
        [DECIMAL_POINT] = ER_NUMBER_MORE_THAN_ONE_DECIMAL_POINT,
        [INVALID]       = ER_NUMBER_INVALID_CHARACTER,
        //[EOF] = ST_NULL, [DIGIT] = ST_NUMBER_DECIMALS
    },
    [ST_NUMBER_DECIMALS] = {
        [EOL]           = ST_NULL,
        [LETTER]        = ER_INVALID_CHARACTER,
        [EXPONENT]      = ST_NUMBER_EXPONENT,
        [DIGIT]         = ST_NUMBER_DECIMALS,
        [OPERATOR]      = ER_INVALID_CHARACTER,
        [PLUS]          = ER_INVALID_CHARACTER,
        [MINUS]         = ER_INVALID_CHARACTER,
        [EQUALS]        = ER_INVALID_CHARACTER,
        [QUOTES]        = ER_INVALID_CHARACTER,
        [DECIMAL_POINT] = ER_NUMBER_MORE_THAN_ONE_DECIMAL_POINT,
        [INVALID]       = ER_NUMBER_INVALID_CHARACTER,
        //[EOF] = ST_NULL, [DIGIT] = ST_NUMBER_DECIMALS, [EXPONENT] = ST_NUMBER_EXPONENT
    },
    [ST_NUMBER_EXPONENT] = {
        [EOL]           = ER_NUMBER_MISSING_EXPONENT_PART,
        [LETTER]        = ER_INVALID_CHARACTER,
        [EXPONENT]      = ER_NUMBER_MORE_THAN_ONE_EXPONENT,
        [DIGIT]         = ST_NUMBER_EXPONENT_AMOUNT,
        [OPERATOR]      = ER_NUMBER_INVALID_CHARACTER,
        [PLUS]          = ST_NUMBER_EXPONENT_SIGN,
        [MINUS]         = ST_NUMBER_EXPONENT_SIGN,
        [EQUALS]        = ER_NUMBER_INVALID_CHARACTER,
        [QUOTES]        = ER_NUMBER_INVALID_CHARACTER,
        [DECIMAL_POINT] = ER_NUMBER_DECIMAL_ON_EXPONENT,
        [INVALID]       = ER_NUMBER_INVALID_CHARACTER,
        //[EOF] = ST_NULL, [DIGIT] = ST_NUMBER_EXPONENT_AMOUNT, [PLUS] = ST_NUMBER_EXPONENT_SIGN, [MINUS] = ST_NUMBER_EXPONENT_SIGN
    },
    [ST_NUMBER_EXPONENT_SIGN] = {
        [EOL]           = ER_NUMBER_MISSING_EXPONENT_PART,
        [LETTER]        = ER_NUMBER_INVALID_CHARACTER,
        [EXPONENT]      = ER_NUMBER_MORE_THAN_ONE_EXPONENT,
        [DIGIT]         = ST_NUMBER_EXPONENT_AMOUNT,
        [OPERATOR]      = ER_NUMBER_INVALID_CHARACTER,
        [PLUS]          = ER_NUMBER_MORE_THAN_ONE_EXPONENT_SIGN,
        [MINUS]         = ER_NUMBER_MORE_THAN_ONE_EXPONENT_SIGN,
        [EQUALS]        = ER_NUMBER_INVALID_CHARACTER,
        [QUOTES]        = ER_NUMBER_INVALID_CHARACTER,
        [DECIMAL_POINT] = ER_NUMBER_DECIMAL_ON_EXPONENT,
        [INVALID]       = ER_NUMBER_INVALID_CHARACTER,
    },
    [ST_NUMBER_EXPONENT_AMOUNT] = {
        [EOL]           = ST_NULL,
        [LETTER]        = ER_NUMBER_INVALID_CHARACTER,
        [EXPONENT]      = ER_NUMBER_MORE_THAN_ONE_EXPONENT,
        [DIGIT]         = ST_NUMBER_EXPONENT_AMOUNT,
        [OPERATOR]      = ER_NUMBER_INVALID_CHARACTER,
        [PLUS]          = ER_NUMBER_INVALID_CHARACTER,
        [MINUS]         = ER_NUMBER_INVALID_CHARACTER,
        [EQUALS]        = ER_NUMBER_INVALID_CHARACTER,
        [QUOTES]        = ER_NUMBER_INVALID_CHARACTER,
        [DECIMAL_POINT] = ER_NUMBER_DECIMAL_ON_EXPONENT,
        [INVALID]       = ER_NUMBER_INVALID_CHARACTER,
        //[EOF] = ST_NULL, [DIGIT] = ST_NUMBER_EXPONENT_AMOUNT
    },

    [ST_STRING_OPEN] = {
        [EOL]           = ER_STRING_LEFT_OPEN,
        [LETTER]        = ST_STRING_OPEN,
        [EXPONENT]      = ST_STRING_OPEN,
        [DIGIT]         = ST_STRING_OPEN,
        [OPERATOR]      = ST_STRING_OPEN,
        [PLUS]          = ST_STRING_OPEN,
        [MINUS]         = ST_STRING_OPEN,
        [EQUALS]        = ST_STRING_OPEN,
        [QUOTES]        = ST_STRING_CLOSED,
        [DECIMAL_POINT] = ST_STRING_OPEN,
        [INVALID]       = ST_STRING_OPEN,
    },
    [ST_STRING_CLOSED] = {
        [EOL]           = ST_NULL,
        [LETTER]        = ER_STRING_ALREADY_CLOSED,
        [EXPONENT]      = ER_STRING_ALREADY_CLOSED,
        [DIGIT]         = ER_STRING_ALREADY_CLOSED,
        [OPERATOR]      = ER_STRING_ALREADY_CLOSED,
        [PLUS]          = ER_STRING_ALREADY_CLOSED,
        [MINUS]         = ER_STRING_ALREADY_CLOSED,
        [EQUALS]        = ER_STRING_ALREADY_CLOSED,
        [QUOTES]        = ER_STRING_ALREADY_CLOSED,
        [DECIMAL_POINT] = ER_STRING_ALREADY_CLOSED,
        [INVALID]       = ER_STRING_ALREADY_CLOSED,
    },

    [ST_OPERATOR_PLUS] = {
        [EOL]           = ST_NULL,
        [LETTER]        = ER_OPERATOR_INVALID_CHARACTER,
        [EXPONENT]      = ER_OPERATOR_INVALID_CHARACTER,
        [DIGIT]         = ER_OPERATOR_INVALID_CHARACTER,
        [OPERATOR]      = ER_OPERATOR_INVALID_COMPLEX,
        [PLUS]          = ST_OPERATOR_INCREMENT,
        [MINUS]         = ER_OPERATOR_INVALID_COMPLEX,
        [EQUALS]        = ST_OPERATOR_COMPLEX,
        [QUOTES]        = ER_OPERATOR_INVALID_CHARACTER,
        [DECIMAL_POINT] = ER_OPERATOR_INVALID_CHARACTER,
        [INVALID]       = ER_OPERATOR_INVALID_CHARACTER,
    },
    [ST_OPERATOR_MINUS] = {
        [EOL]           = ST_NULL,
        [LETTER]        = ER_OPERATOR_INVALID_CHARACTER,
        [EXPONENT]      = ER_OPERATOR_INVALID_CHARACTER,
        [DIGIT]         = ST_NUMBER,
        [OPERATOR]      = ER_OPERATOR_INVALID_COMPLEX,
        [PLUS]          = ER_OPERATOR_INVALID_COMPLEX,
        [MINUS]         = ST_OPERATOR_DECREMENT,
        [EQUALS]        = ST_OPERATOR_COMPLEX,
        [QUOTES]        = ER_OPERATOR_INVALID_CHARACTER,
        [DECIMAL_POINT] = ER_OPERATOR_INVALID_CHARACTER,
        [INVALID]       = ER_OPERATOR_INVALID_CHARACTER
    },
    [ST_OPERATOR_OTHER] = {
        [EOL]           = ST_NULL,
        [LETTER]        = ER_OPERATOR_INVALID_CHARACTER,
        [EXPONENT]      = ER_OPERATOR_INVALID_CHARACTER,
        [DIGIT]         = ER_OPERATOR_INVALID_CHARACTER,
        [OPERATOR]      = ST_OPERATOR_COMPLEX,
        [PLUS]          = ER_OPERATOR_INVALID_CHARACTER,
        [MINUS]         = ER_OPERATOR_INVALID_CHARACTER,
        [EQUALS]        = ST_OPERATOR_COMPLEX,
        [QUOTES]        = ER_OPERATOR_INVALID_CHARACTER,
        [DECIMAL_POINT] = ER_OPERATOR_INVALID_CHARACTER,
        [INVALID]       = ER_OPERATOR_INVALID_CHARACTER,

    },
    [ST_OPERATOR_INCREMENT] = {
        [EOL]           = ST_NULL,
        [LETTER]        = ER_OPERATOR_TOO_LONG,
        [EXPONENT]      = ER_OPERATOR_TOO_LONG,
        [DIGIT]         = ER_OPERATOR_TOO_LONG,
        [OPERATOR]      = ER_OPERATOR_TOO_LONG,
        [PLUS]          = ER_OPERATOR_TOO_LONG,
        [MINUS]         = ER_OPERATOR_TOO_LONG,
        [EQUALS]        = ER_OPERATOR_TOO_LONG,
        [QUOTES]        = ER_OPERATOR_TOO_LONG,
        [DECIMAL_POINT] = ER_OPERATOR_TOO_LONG,
        [INVALID]       = ER_OPERATOR_TOO_LONG,

    },
    [ST_OPERATOR_DECREMENT] = {
        [EOL]           = ST_NULL,
        [LETTER]        = ER_OPERATOR_TOO_LONG,
        [EXPONENT]      = ER_OPERATOR_TOO_LONG,
        [DIGIT]         = ER_OPERATOR_TOO_LONG,
        [OPERATOR]      = ER_OPERATOR_TOO_LONG,
        [PLUS]          = ER_OPERATOR_TOO_LONG,
        [MINUS]         = ER_OPERATOR_TOO_LONG,
        [EQUALS]        = ER_OPERATOR_TOO_LONG,
        [QUOTES]        = ER_OPERATOR_TOO_LONG,
        [DECIMAL_POINT] = ER_OPERATOR_TOO_LONG,
        [INVALID]       = ER_OPERATOR_TOO_LONG,

    },
    [ST_OPERATOR_COMPLEX] = {
        [EOL]           = ST_NULL,
        [LETTER]        = ER_OPERATOR_TOO_LONG,
        [EXPONENT]      = ER_OPERATOR_TOO_LONG,
        [DIGIT]         = ER_OPERATOR_TOO_LONG,
        [OPERATOR]      = ER_OPERATOR_TOO_LONG,
        [PLUS]          = ER_OPERATOR_TOO_LONG,
        [MINUS]         = ER_OPERATOR_TOO_LONG,
        [EQUALS]        = ER_OPERATOR_TOO_LONG,
        [QUOTES]        = ER_OPERATOR_TOO_LONG,
        [DECIMAL_POINT] = ER_OPERATOR_TOO_LONG,
        [INVALID]       = ER_OPERATOR_TOO_LONG,
    },

    [ST_IDENTIFIER] = {
        [EOL]           = ST_NULL,
        [LETTER]        = ST_IDENTIFIER,
        [EXPONENT]      = ST_IDENTIFIER,
        [DIGIT]         = ST_IDENTIFIER_WITH_NUMBERS,
        [OPERATOR]      = ER_IDENTIFIER_INVALID_CHARACTER,
        [PLUS]          = ER_IDENTIFIER_INVALID_CHARACTER,
        [MINUS]         = ER_IDENTIFIER_INVALID_CHARACTER,
        [EQUALS]        = ER_IDENTIFIER_INVALID_CHARACTER,
        [QUOTES]        = ER_IDENTIFIER_INVALID_CHARACTER,
        [DECIMAL_POINT] = ER_IDENTIFIER_INVALID_CHARACTER,
        [INVALID]       = ER_IDENTIFIER_INVALID_CHARACTER,
    },
    [ST_IDENTIFIER_WITH_NUMBERS] = {
        [EOL]           = ST_NULL,
        [LETTER]        = ER_IDENTIFIER_LETTER_AFTER_DIGIT,
        [EXPONENT]      = ER_IDENTIFIER_LETTER_AFTER_DIGIT,
        [DIGIT]         = ST_IDENTIFIER_WITH_NUMBERS,
        [OPERATOR]      = ER_IDENTIFIER_INVALID_CHARACTER,
        [PLUS]          = ER_IDENTIFIER_INVALID_CHARACTER,
        [MINUS]         = ER_IDENTIFIER_INVALID_CHARACTER,
        [EQUALS]        = ER_IDENTIFIER_INVALID_CHARACTER,
        [QUOTES]        = ER_IDENTIFIER_INVALID_CHARACTER,
        [DECIMAL_POINT] = ER_IDENTIFIER_INVALID_CHARACTER,
        [INVALID]       = ER_IDENTIFIER_INVALID_CHARACTER,
    },
};

static bool isDigit(char c);
static bool isOperator(char c);
static bool isLetter(char c);
static SymbolType getSymbolType(char symbol);

static bool isKeyword(const char* word);

static bool isError(int state);
static void printError(int error, const char* input, char last_char);

LexerResult parseLexeme(const char* input, TokenType* token_type) {
    LexerState state   = ST_START;
    LexerResult result = LEX_SUCCESS;

    const char* c = input;
    while (true) {
        const SymbolType type       = getSymbolType(*c);
        const LexerState next_state = lexer_matrix[state][type];

        if (isError(next_state)) {
            printError(next_state, input, *c);
            result = (LexerResult)next_state;
            break;
        }

        if (*c) {
            state = next_state;
            c++;
        } else break;
    }

    // Set token type
    if (token_type == nullptr) return result;

    if (state >= ST_NUMBER && state <= ST_NUMBER_EXPONENT_AMOUNT) {
        *token_type = TOKEN_NUMBER;
    } else if (state >= ST_STRING_OPEN && state <= ST_STRING_CLOSED) {
        *token_type = TOKEN_STRING;
    } else if (state >= ST_OPERATOR_PLUS && state <= ST_OPERATOR_COMPLEX) {
        *token_type = TOKEN_OPERATOR;
    } else if (state >= ST_IDENTIFIER && state <= ST_IDENTIFIER_WITH_NUMBERS) {
        *token_type = isKeyword(input) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
    } else {
        *token_type = TOKEN_INVALID;
    }
    return result;
}

#pragma region Get character category
static SymbolType getSymbolType(const char symbol) {
    switch (symbol) {
        case '\0': return EOL;
        case '+': return PLUS;
        case '-': return MINUS;
        case '=': return EQUALS;
        case '.': return DECIMAL_POINT;
        case 'E':
        case 'e': return EXPONENT;
        case '"': return QUOTES;
        default: ;
    }

    if (isDigit(symbol)) return DIGIT;
    if (isOperator(symbol)) return OPERATOR;
    if (isLetter(symbol)) return LETTER;

    return INVALID;
}

bool isDigit(const char c) {
    return c >= '0' && c <= '9';
}

bool isOperator(const char c) {
    constexpr char operators[]  = {'+', '-', '*', '/', '%', '=', '<', '>'};
    constexpr int operator_size = sizeof(operators) / sizeof(operators[0]);

    for (int i = 0; i < operator_size; i++) {
        if (c == operators[i]) return true;
    }

    return false;
}

bool isLetter(const char c) {
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '_';
}
#pragma endregion

static bool isKeyword(const char* word) {
    static const char* keywords[] = {
        "if",
        "else",

        "switch",
        "case",
        "break",
        "default",

        "for",
        "while",
        "continue",

        "void",
        "unsigned",
        "char",
        "int",
        "float",
        "double",
        "bool",

        "return",
        "const",
        "static",
        "volatile",
        "sizeof",
        "goto",

        "enum",
        "struct",
        "union",
        "typedef",
    };
    static constexpr int keyword_count = sizeof(keywords) / sizeof(keywords[0]);

    for (int i = 0; i < keyword_count; i++) {
        if (strcmp(keywords[i], word) == 0) return true;
    }

    return false;
}

static bool isError(const int state) {
    return state >= ERROR_OFFSET && state < ERROR_COUNT;
}

static void printError(int error, const char* input, char last_char) {
    const char* error_messages[] = {
        [ER_IDK]                                = "",
        [ER_EMPTY]                              = "",
        [ER_INVALID_CHARACTER]                  = "",
        [ER_NUMBER_MISSING_INTEGER_PART]        = "",
        [ER_NUMBER_MISSING_DECIMAL_PART]        = "",
        [ER_NUMBER_MISSING_EXPONENT_PART]       = "",
        [ER_NUMBER_MORE_THAN_ONE_DECIMAL_POINT] = "",
        [ER_NUMBER_DECIMAL_ON_EXPONENT]         = "",
        [ER_NUMBER_MORE_THAN_ONE_EXPONENT]      = "",
        [ER_NUMBER_MORE_THAN_ONE_EXPONENT_SIGN] = "",
        [ER_NUMBER_INVALID_CHARACTER]           = "",
        [ER_STRING_LEFT_OPEN]                   = "",
        [ER_STRING_ALREADY_CLOSED]              = "",
        [ER_OPERATOR_TOO_LONG]                  = "",
        [ER_OPERATOR_INVALID_COMPLEX]           = "",
        [ER_OPERATOR_INVALID_CHARACTER]         = "",
        [ER_IDENTIFIER_INVALID_CHARACTER]       = "",
        [ER_IDENTIFIER_LETTER_AFTER_DIGIT]      = ""
    };

    if (!isError(error)) error = ER_IDK;
    const char* errormsg = error_messages[error - ERROR_OFFSET];
    printf("Algo salio mal. Código de error: %d\n", error);
    printf(errormsg, input, last_char);
}
