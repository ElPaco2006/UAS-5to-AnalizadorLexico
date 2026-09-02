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

    //ST_STRING_OPEN    = 8,
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
} State;

static int lexer_matrix[STATE_COUNT][SYMBOL_COUNT] = {
    [ST_NULL]  = {0},
    [ST_START] = {
        [EOF]           = ST_NULL,
        [LETTER]        = ST_IDENTIFIER,
        [EXPONENT]      = ST_IDENTIFIER,
        [DIGIT]         = ST_NUMBER,
        [OPERATOR]      = ST_OPERATOR_OTHER,
        [PLUS]          = ST_OPERATOR_PLUS,
        [MINUS]         = ST_OPERATOR_MINUS,
        [EQUALS]        = ST_OPERATOR_OTHER,
        [QUOTES]        = ST_STRING_OPEN,
        [DECIMAL_POINT] = ER_IDK,
        [INVALID]       = ER_IDK,
    },

    [ST_NUMBER] = {
        [EOF]           = ST_NULL,
        [LETTER]        = ER_IDK,
        [EXPONENT]      = ST_NUMBER_EXPONENT,
        [DIGIT]         = ST_NUMBER,
        [OPERATOR]      = ER_IDK,
        [PLUS]          = ER_IDK,
        [MINUS]         = ER_IDK,
        [EQUALS]        = ER_IDK,
        [QUOTES]        = ER_IDK,
        [DECIMAL_POINT] = ST_NUMBER_POINT,
        [INVALID]       = ER_IDK,
    },
    [ST_NUMBER_POINT] = {
        [EOF]           = ER_IDK,
        [LETTER]        = ER_IDK,
        [EXPONENT]      = ER_IDK,
        [DIGIT]         = ST_NUMBER_DECIMALS,
        [OPERATOR]      = ER_IDK,
        [PLUS]          = ER_IDK,
        [MINUS]         = ER_IDK,
        [EQUALS]        = ER_IDK,
        [QUOTES]        = ER_IDK,
        [DECIMAL_POINT] = ER_IDK,
        [INVALID]       = ER_IDK,
        //[EOF] = ST_NULL, [DIGIT] = ST_NUMBER_DECIMALS
    },
    [ST_NUMBER_DECIMALS] = {
        [EOF]           = ST_NULL,
        [LETTER]        = ER_IDK,
        [EXPONENT]      = ST_NUMBER_EXPONENT,
        [DIGIT]         = ST_NUMBER_DECIMALS,
        [OPERATOR]      = ER_IDK,
        [PLUS]          = ER_IDK,
        [MINUS]         = ER_IDK,
        [EQUALS]        = ER_IDK,
        [QUOTES]        = ER_IDK,
        [DECIMAL_POINT] = ER_IDK,
        [INVALID]       = ER_IDK,
        //[EOF] = ST_NULL, [DIGIT] = ST_NUMBER_DECIMALS, [EXPONENT] = ST_NUMBER_EXPONENT
    },
    [ST_NUMBER_EXPONENT] = {
        [EOF]           = ER_IDK,
        [LETTER]        = ER_IDK,
        [EXPONENT]      = ER_IDK,
        [DIGIT]         = ST_NUMBER_EXPONENT_AMOUNT,
        [OPERATOR]      = ER_IDK,
        [PLUS]          = ST_NUMBER_EXPONENT_SIGN,
        [MINUS]         = ST_NUMBER_EXPONENT_SIGN,
        [EQUALS]        = ER_IDK,
        [QUOTES]        = ER_IDK,
        [DECIMAL_POINT] = ER_IDK,
        [INVALID]       = ER_IDK,
        //[EOF] = ST_NULL, [DIGIT] = ST_NUMBER_EXPONENT_AMOUNT, [PLUS] = ST_NUMBER_EXPONENT_SIGN, [MINUS] = ST_NUMBER_EXPONENT_SIGN
    },
    [ST_NUMBER_EXPONENT_SIGN] = {
        [EOF]           = ER_IDK,
        [LETTER]        = ER_IDK,
        [EXPONENT]      = ER_IDK,
        [DIGIT]         = ST_NUMBER_EXPONENT_AMOUNT,
        [OPERATOR]      = ER_IDK,
        [PLUS]          = ER_IDK,
        [MINUS]         = ER_IDK,
        [EQUALS]        = ER_IDK,
        [QUOTES]        = ER_IDK,
        [DECIMAL_POINT] = ER_IDK,
        [INVALID]       = ER_IDK,
    },
    [ST_NUMBER_EXPONENT_AMOUNT] = {
        [EOF]           = ST_NULL,
        [LETTER]        = ER_IDK,
        [EXPONENT]      = ER_IDK,
        [DIGIT]         = ST_NUMBER_EXPONENT_AMOUNT,
        [OPERATOR]      = ER_IDK,
        [PLUS]          = ER_IDK,
        [MINUS]         = ER_IDK,
        [EQUALS]        = ER_IDK,
        [QUOTES]        = ER_IDK,
        [DECIMAL_POINT] = ER_IDK,
        [INVALID]       = ER_IDK,
        //[EOF] = ST_NULL, [DIGIT] = ST_NUMBER_EXPONENT_AMOUNT
    },

    [ST_STRING_OPEN] = {
        [EOF]           = ER_STRING_LEFT_OPEN,
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
        [EOF]           = ST_NULL,
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
        [EOF]           = ST_NULL,
        [LETTER]        = ER_IDK,
        [EXPONENT]      = ER_IDK,
        [DIGIT]         = ER_IDK,
        [OPERATOR]      = ER_IDK,
        [PLUS]          = ST_OPERATOR_INCREMENT,
        [MINUS]         = ER_IDK,
        [EQUALS]        = ST_OPERATOR_COMPLEX,
        [QUOTES]        = ER_IDK,
        [DECIMAL_POINT] = ER_IDK,
        [INVALID]       = ER_IDK,
    },
    [ST_OPERATOR_MINUS] = {
        [EOF]           = ST_NULL,
        [LETTER]        = ER_IDK,
        [EXPONENT]      = ER_IDK,
        [DIGIT]         = ST_NUMBER,
        [OPERATOR]      = ER_IDK,
        [PLUS]          = ER_IDK,
        [MINUS]         = ST_OPERATOR_DECREMENT,
        [EQUALS]        = ST_OPERATOR_COMPLEX,
        [QUOTES]        = ER_IDK,
        [DECIMAL_POINT] = ER_IDK,
        [INVALID]       = ER_IDK
    },
    [ST_OPERATOR_OTHER] = {
        [EOF]           = ST_NULL,
        [LETTER]        = ER_IDK,
        [EXPONENT]      = ER_IDK,
        [DIGIT]         = ER_IDK,
        [OPERATOR]      = ER_IDK,
        [PLUS]          = ER_IDK,
        [MINUS]         = ER_IDK,
        [EQUALS]        = ST_OPERATOR_COMPLEX,
        [QUOTES]        = ER_IDK,
        [DECIMAL_POINT] = ER_IDK,
        [INVALID]       = ER_IDK,

    },
    [ST_OPERATOR_INCREMENT] = {
        [EOF]           = ST_NULL,
        [LETTER]        = ER_IDK,
        [EXPONENT]      = ER_IDK,
        [DIGIT]         = ER_IDK,
        [OPERATOR]      = ER_IDK,
        [PLUS]          = ER_IDK,
        [MINUS]         = ER_IDK,
        [EQUALS]        = ER_IDK,
        [QUOTES]        = ER_IDK,
        [DECIMAL_POINT] = ER_IDK,
        [INVALID]       = ER_IDK,

    },
    [ST_OPERATOR_DECREMENT] = {
        [EOF]           = ST_NULL,
        [LETTER]        = ER_IDK,
        [EXPONENT]      = ER_IDK,
        [DIGIT]         = ER_IDK,
        [OPERATOR]      = ER_IDK,
        [PLUS]          = ER_IDK,
        [MINUS]         = ER_IDK,
        [EQUALS]        = ER_IDK,
        [QUOTES]        = ER_IDK,
        [DECIMAL_POINT] = ER_IDK,
        [INVALID]       = ER_IDK,

    },
    [ST_OPERATOR_COMPLEX] = {
        [EOF]           = ST_NULL,
        [LETTER]        = ER_IDK,
        [EXPONENT]      = ER_IDK,
        [DIGIT]         = ER_IDK,
        [OPERATOR]      = ER_IDK,
        [PLUS]          = ER_IDK,
        [MINUS]         = ER_IDK,
        [EQUALS]        = ER_IDK,
        [QUOTES]        = ER_IDK,
        [DECIMAL_POINT] = ER_IDK,
        [INVALID]       = ER_IDK,
    },

    [ST_IDENTIFIER] = {
        [EOF]           = ST_NULL,
        [LETTER]        = ST_IDENTIFIER,
        [EXPONENT]      = ST_IDENTIFIER,
        [DIGIT]         = ST_IDENTIFIER_WITH_NUMBERS,
        [OPERATOR]      = ER_IDK,
        [PLUS]          = ER_IDK,
        [MINUS]         = ER_IDK,
        [EQUALS]        = ER_IDK,
        [QUOTES]        = ER_IDK,
        [DECIMAL_POINT] = ER_IDK,
        [INVALID]       = ER_IDK,
    },
    [ST_IDENTIFIER_WITH_NUMBERS] = {
        [EOF]           = ST_NULL,
        [LETTER]        = ER_IDENTIFIER_LETTER_AFTER_DIGIT,
        [EXPONENT]      = ER_IDENTIFIER_LETTER_AFTER_DIGIT,
        [DIGIT]         = ST_IDENTIFIER_WITH_NUMBERS,
        [OPERATOR]      = ER_IDK,
        [PLUS]          = ER_IDK,
        [MINUS]         = ER_IDK,
        [EQUALS]        = ER_IDK,
        [QUOTES]        = ER_IDK,
        [DECIMAL_POINT] = ER_IDK,
        [INVALID]       = ER_IDK,
    },
};
static char* error_msg[ERROR_COUNT] = {0};

static bool isDigit(char c);
static bool isOperator(char c);
static bool isLetter(char c);

static SymbolType getSymbolType(char symbol);
static bool isKeyword(const char* word);

static bool isError(int state);
static void printError(int error, const char* input);

LexerResult parseLexeme(const char* input, TokenType* token_type) {
    State state        = ST_START;
    LexerResult result = LEX_SUCCESS;

    const char* c = input;
    while (true) {
        const SymbolType type  = getSymbolType(*c);
        const State next_state = lexer_matrix[state][type];

        if (isError(next_state)) {
            printError(next_state, input);
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
    constexpr char operators[]  = {'+', '-', '*', '/', '%', '=', '<', '>'};
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

static void printError(int error, const char* input) {}
