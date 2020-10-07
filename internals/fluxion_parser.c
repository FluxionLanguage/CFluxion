//
// Created by Ege Ozkan on 5.10.2020.
//

#include "fluxion_parser.h"
#include <ctype.h>
#include <string.h>

TokenStack *initTokenStack() {
    TokenStack  *stack = (TokenStack *) malloc(sizeof(TokenStack));
    stack->current = 0;
    stack->capacity = 8;
    stack->tokens = (Token**) malloc(sizeof(Token) * stack->capacity);
    return stack;
}


void StackPush(TokenStack *stack, Token *token) {
    if (stack->current >= stack->capacity) {
        stack->capacity *= 2;
        stack->tokens = (Token**) realloc(stack->tokens, sizeof(Token*) * stack->capacity);
    }
    stack->tokens[stack->current++] = token;
}

Token *StackPop(TokenStack *stack, Token *token) {
    if (stack->current == 0) {
        return NULL;
    } else {
        return stack->tokens[stack->current--];
    }
}

void parserConsume(Parser *parser) {
    if (parser->ch_ != 0)
        parser->ch_++;
}

char parserPeek(Parser *parser) {
    return *parser->ch_;
}
char parserPop(Parser *parser) {
    char c = parserPeek(parser);
    parser->ch_++;
    return c;
}

bool isDigit(Parser *parser) {
    return isdigit(parserPeek(parser)) == 1;
}

bool isWhitespace(Parser *parser) {
    return parserPeek(parser) == ' ' || parserPeek(parser) == '\t';
}

bool isEOL(Parser *parser) {
    return parserPeek(parser) == '\n' && !parser->ignoreEOL;
}

NumberToken *parseNumber(Parser *parser) {
    char *number = "";
    while (!isEOL(parser) && !isWhitespace(parser) && (isDigit(parser) || parserPeek(parser) == '.')) {
        strncat(number, parser->ch_, 1);
        parserConsume(parser);
    }
    return initNumberToken(parser->lineCount, strtod(number, NULL));
}