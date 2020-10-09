//
// Created by Ege Ozkan on 5.10.2020.
//

#ifndef FLUXIONCORE_FLUXION_PARSER_H
#define FLUXIONCORE_FLUXION_PARSER_H
#include "fluxion_token.h"


typedef struct {
    Token **tokens;
    int current;
    int capacity;
} TokenStack;

TokenStack *initTokenStack();
void StackPush(TokenStack *stack, Token *token);
Token *StackPop(TokenStack *stack, Token *token);

typedef struct {
    const char *source;
    char *ch_;
    bool ignoreEOL;
    int lineCount;
    TokenStack *stack;
} Parser;


void parserConsume(Parser *parser);
char parserPeek(Parser *parser);
char parserPop(Parser *parser);
char parserDoublePeek(Parser *parser);
void parserRewind(Parser *parser);

void issueParserError(Parser *parser, ErrorLiteral literal, const char *message);

bool isDigit(Parser *parser);
bool isWhitespace(Parser *parser);
bool isEOL(Parser *Parser);

/**
 * Parse an expression.
 * Push it to the token stack.
 * @param parser
 * @param terminal Character the expression will end on.
 */
ExpressionToken *parseExpression(Parser *parser, const char *terminal);
Parser *parse(const char *source);
Token **getTokens(Parser *parser);
int getTokenCount(Parser *parser);
#endif //FLUXIONCORE_FLUXION_PARSER_H
