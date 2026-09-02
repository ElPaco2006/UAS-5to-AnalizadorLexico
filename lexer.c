//
// Created by paco on 01/09/26.
//

#include <string.h>
#include "lexer.h"

typedef enum {
    EOF, // \0

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

    ST_STRING_OPEN    = 8,
    ST_STRING_CONTENT = 9,
    ST_STRING_CLOSE   = 10,

    ST_OPERATOR_PLUS      = 11,
    ST_OPERATOR_MINUS     = 12,
    ST_OPERATOR_OTHER     = 13,
    ST_OPERATOR_INCREMENT = 14,
    ST_OPERATOR_DECREMENT = 15,
    ST_OPERATOR_COMPLEX   = 16,

    ST_WORD              = 17,
    ST_WORD_WITH_NUMBERS = 18,

    STATE_COUNT,
} State;

static constexpr int ERROR_OFFSET = 1000;

typedef enum {
    ER_UNKNOWN,
    ERROR_COUNT
} Error;

static int lexer_matrix[STATE_COUNT][SYMBOL_COUNT] = {
    [ST_NULL]                   = {0},
    [ST_START]                  = {[EOF] = ST_NULL, [DIGIT] = ST_NUMBER, [MINUS] = ST_OPERATOR_MINUS},
    [ST_NUMBER]                 = {[EOF] = ST_NULL, [DIGIT] = ST_NUMBER, [DECIMAL_POINT] = ST_NUMBER_POINT, [EXPONENT] = ST_NUMBER_EXPONENT},
    [ST_NUMBER_POINT]           = {[EOF] = ST_NULL, [DIGIT] = ST_NUMBER_DECIMALS},
    [ST_NUMBER_DECIMALS]        = {[EOF] = ST_NULL, [DIGIT] = ST_NUMBER_DECIMALS, [EXPONENT] = ST_NUMBER_EXPONENT},
    [ST_NUMBER_EXPONENT]        = {[EOF] = ST_NULL, [DIGIT] = ST_NUMBER_EXPONENT_AMOUNT, [PLUS] = ST_NUMBER_EXPONENT_SIGN, [MINUS] = ST_NUMBER_EXPONENT_SIGN},
    [ST_NUMBER_EXPONENT_SIGN]   = {[EOF] = ST_NULL, [DIGIT] = ST_NUMBER_EXPONENT_AMOUNT},
    [ST_NUMBER_EXPONENT_AMOUNT] = {[EOF] = ST_NULL, [DIGIT] = ST_NUMBER_EXPONENT_AMOUNT},
    [ST_OPERATOR_MINUS]         = {[EOF] = ST_NULL, [DIGIT] = ST_NUMBER},
};

static char* keywords[] = {
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

static char* error_msg[ERROR_COUNT] = {0};

static bool isDigit(char c);
static bool isOperator(char c);
static bool isLetter(char c);

static SymbolType getSymbolType(char symbol);
static bool isKeyword(const char* word);

static bool isError(int state);
static void printError(int error, const char* input);

bool parseLexeme(const char* input, TokenType* token_type) {
    int state           = ST_START;
    bool is_valid_token = true;

    const char* c = input;
    while (true) {
        const SymbolType type  = getSymbolType(*c);
        const State next_state = lexer_matrix[state][type];

        if (isError(next_state)) {
            printError(next_state, input);
            is_valid_token = false;
            break;
        }

        if (*c) {
            state = next_state;
            c++;
        } else break;
    }

    // Set token type
    if (token_type == nullptr) return is_valid_token;

    if (state >= ST_NUMBER && state <= ST_NUMBER_EXPONENT_AMOUNT) {
        *token_type = TOKEN_NUMBER;
    } else if (state >= ST_STRING_OPEN && state <= ST_STRING_CLOSE) {
        *token_type = TOKEN_STRING;
    } else if (state >= ST_OPERATOR_PLUS && state <= ST_OPERATOR_COMPLEX) {
        *token_type = TOKEN_OPERATOR;
    } else if (state >= ST_WORD && state <= ST_WORD_WITH_NUMBERS) {
        *token_type = isKeyword(input) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
    } else {
        *token_type = TOKEN_INVALID;
    }
    return is_valid_token;
}

static SymbolType getSymbolType(const char symbol) {
    switch (symbol) {
        case '\0': return EOF;
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

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool isOperator(const char c) {
    constexpr char operators[]  = {'+', '-', '*', '/', '=', '!', '<', '>'};
    constexpr int operator_size = sizeof(operators) / sizeof(operators[0]);

    for (int i = 0; i < operator_size; i++) {
        if (c == operators[i]) return true;
    }

    return false;
}

bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '_';
}

static bool isKeyword(const char* word) {
    for (int i = 0; i < keyword_count; i++) {
        if (strcmp(keywords[i], word) == 0) return true;
    }

    return false;
}

static bool isError(const int state) {
    return false;
    //return state >= 0 && state < STATE_COUNT;
}

static void printError(int error, const char* input) {}
