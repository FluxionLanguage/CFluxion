//
// Created by Ege Ozkan on 5.10.2020.
//

#include "fluxion_parser.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

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

char parserDoublePeek(Parser *parser) {
    if (*parser->ch_ != 0) {
        return *(parser->ch_ + 1);
    } else {
        return 0;
    }
}
void parserRewind(Parser *parser) {
    parser->ch_--;
}

void issueParserError(Parser *parser, ErrorLiteral literal, const char *message) {
    char *str = NULL;
    sprintf(str, "at line %i, %s", parser->lineCount, message);
    Error newError = {literal, str};
    issueError(&newError);
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

bool isCharTerminator(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\0';
}

NumberToken *parseNumber(Parser *parser) {
    char *number = "";
    while (!isEOL(parser) && !isWhitespace(parser) && (isDigit(parser) || parserPeek(parser) == '.')) {
        strncat(number, parser->ch_, 1);
        parserConsume(parser);
    }
    return initNumberToken(parser->lineCount, strtod(number, NULL));
}

OperatorToken *parseOperator(Parser *parser) {
    char firstCharacter = parserPop(parser); // Some operators take two! we need to check.
    char nextCharacter = parserPeek(parser);
    OperatorToken *opToken;
    switch (firstCharacter) { // The operator trie.
        case '+':
            return initOperatorToken(parser->lineCount, PLUS);
        case '-':
            if (nextCharacter == '>') {
                parserConsume(parser);
                return initOperatorToken(parser->lineCount, LIMIT);
            }
            return initOperatorToken(parser->lineCount, MINUS);
        case '\\':
            if (nextCharacter == '=') {
                parserConsume(parser);
                opToken = initOperatorToken(parser->lineCount, NEQ);
            }
            return initOperatorToken(parser->lineCount, NOT);
        case '*':
            return initOperatorToken(parser->lineCount, MULTIPLY);
        case '/':
            return initOperatorToken(parser->lineCount, DIVIDE);
        case '&':
            return initOperatorToken(parser->lineCount, AMPERSAND);
        case '<':
            if (nextCharacter == '=') {
                parserConsume(parser);
                return initOperatorToken(parser->lineCount, LEQ);
            }
            return initOperatorToken(parser->lineCount, LESS);
        case '>':
            if (nextCharacter == '=') {
                parserConsume(parser);
                return initOperatorToken(parser->lineCount, GEQ);
            }
            return initOperatorToken(parser->lineCount, GREATER);
        case 'i':
            if (nextCharacter == 'n' && isCharTerminator(parserDoublePeek(parser))) { // By the way, this line
                // Turns the parser into a 2 lookahead parser... So that's a huge performance issue but anyway...
                return initOperatorToken(parser->lineCount, IN);
            } else { // We now need to put the i back.
                parserRewind(parser);
                return NULL; // And return null.
            }
        case '=':
            return initOperatorToken(parser->lineCount, EQUAL);
        case ':':
            switch (nextCharacter) {
                case ':':
                    parserConsume(parser);
                    return initOperatorToken(parser->lineCount, SCOPE);
                case '=':
                    parserConsume(parser);
                    return initOperatorToken(parser->lineCount, ASSIGN);
                default: // We need to error out! This is a reserved but invalid token!
                    issueParserError(parser, Undefined, ": operator is not defined on any type.");
                    return NULL;
            }
        case '!':
            return initOperatorToken(parser->lineCount, FACTORIAL);
        case '_':
            return initOperatorToken(parser->lineCount, GET);
        case '^':
            return initOperatorToken(parser->lineCount, POWER);
    }
}


ExpressionToken *parseExpression(Parser *parser, char terminal) {
    TokenStack *tokenStack = initTokenStack(); // A stack just for this expression.
    Token *token = NULL; // Traversing token pointer;
    while (parserPeek(parser) != terminal) {
        char ch = parserPeek(parser);
        if (isWhitespace(parser)) {
            continue; // Whitespaces not caught by another rule is cast aside.
        }
        switch (ch) {
            case '+':
            case '-':
            case '\\':
            case '*':
            case '/':
            case '&':
            case '<':
            case '>':
            case 'i':
            case '=':
            case ':':
            case '!':
            case '\'':
            case '_':
            case '^':
                ; // In C, a case cannot immediately have a definition.
                token = (Token*) parseOperator(parser);
                break;
            case '(':
                parserConsume(parser); // Consume (.
                token = (Token*) parseExpression(parser, ')');
                break;
        }
        if (token != NULL) {
            StackPush(tokenStack, (Token*) token);
        }
    }
}