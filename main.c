#include <stdio.h>
#include <string.h>

#include "lexer.h"

int main() {
    printf("================== Paco Compiler ==================\n");
    printf("Análizador léxico\n");
    while (true) {
        constexpr int BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE]  = {0};
        printf("Entrada: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        buffer[BUFFER_SIZE - 1] = '\0';
        const size_t len        = strlen(buffer);
        if (len > 0) buffer[len - 1] = '\0';


        TokenType token_type     = TOKEN_INVALID;
        const LexerResult result = parseLexeme(buffer, &token_type);
        printf("Tipo de token: %s\n", getTokenTypeName(token_type));
    }
    return 0;
}
