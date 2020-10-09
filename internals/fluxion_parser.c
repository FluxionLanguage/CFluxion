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

bool isCharInStr(char c, const char *str) {
    char *ptr = str;
    while (*ptr) {
        if (c == *ptr) {
            return true;
        }
        ptr++;
    }
    return false;
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

/**
 * Consume a singleline comment, discard it.
 * @param parser Parser pointer.
 */
void consumeComment(Parser *parser) {
    while (parserPeek(parser) != '\n') {
        parserConsume(parser);
    }
}

void consumeMultilineComment(Parser *parser) {
    do {
        char c = parserPop(parser);
        if (c == '*' && parserPeek(parser) == ';') {
            parserConsume(parser);
            break;
        }
    } while (true);
}

void parseFunctionArgs(Parser *parser, FunctionToken *functionToken) {
    while (parserPeek(parser) != ')') {
        if (parser->ignoreEOL && parserPeek(parser) != '\n') {
            issueParserError(parser, Undefined, "Expected new line.");
        }
        switch (parserPeek(parser)) {
            case '\0':
                issueParserError(parser, Undefined, "Expected ).");
            case '\\':
                if (parserDoublePeek(parser) == '\\') { // Might be line converter.
                    parser->ignoreEOL = true;
                    parserConsume(parser);
                    parserConsume(parser);
                } else {
                    addArgument(functionToken, (Token *) parseExpression(parser, ",)")); // Might be start of not.
                }
                break;
            case ';':
                parserConsume(parser);
                switch (parserDoublePeek(parser)) {
                    case ';':
                        parserConsume(parser);
                        consumeComment(parser);
                        break;
                    case '*':
                        parserConsume(parser);
                        consumeMultilineComment(parser);
                        break;
                    default:
                        issueParserError(parser, Undefined, "Expected ; or *");
                        break;
                }
                break;
            case '\n':
                if (parser->ignoreEOL) {
                    parser->ignoreEOL = false;
                    parserConsume(parser);
                } else {
                    issueParserError(parser, Undefined, "Expected )");
                }
                break;
            default:
                addArgument(functionToken, (Token *) parseExpression(parser, ",)"));
        }

    }
}

ExpressionToken *parseExpression(Parser *parser, const char *terminals) {
    TokenStack *tokenStack = initTokenStack(); // A stack just for this expression.
    Token *token = NULL; // Traversing token pointer;
    while (isCharInStr(parserPeek(parser), terminals)) {
        char ch = parserPeek(parser);
        if (isWhitespace(parser)) {
            if (token != NULL && token->tokenType == IDENTIFIER) {//If identifier
                if (((IdentifierToken *) token)->identifierType == Function) {
                    finaliseFunctionToken((FunctionToken*) token);
                }
                StackPush(tokenStack, token);
                token = NULL; // We finalised the identifier.
            }
            continue; // Whitespaces not caught by another rule is cast aside.
        } else if (parser->ignoreEOL && ch != '\n') {
            issueParserError(parser, Undefined, "Expected new line.");
        }
        switch (ch) {
            case '+':
            case '-':
            case '\\':
                if (parserDoublePeek(parser) == '\\') {
                    if (parser->ignoreEOL) {
                        issueParserError(parser, Undefined, "Expected new line.");
                    }
                    parser->ignoreEOL = true;
                    break;
                }
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
                if (token == NULL) {
                    parserConsume(parser); // Consume (.
                    token = (Token *) parseExpression(parser, ")");
                } else { // Otherwise, call list for function.
                    // First, convert the identifer to a Function.
                    FunctionToken *newFunc = initFunctionToken(token->lineCount, ((IdentifierToken*) token)->name);
                    freeToken(token);
                    parseFunctionArgs(parser, newFunc);
                    token = (Token *) newFunc;
                }
                break;
            default: // Identifier
                if (token == NULL) {
                    token = (Token*) initIdentifierToken(parser->lineCount, "");
                }
                strncpy(((IdentifierToken*) token)->name, parser->ch_, 1);
                continue;
        }
        if (token != NULL) {
            StackPush(tokenStack, (Token*) token);
            token = NULL;
        }
    }
}