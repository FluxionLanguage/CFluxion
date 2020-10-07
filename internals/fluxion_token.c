//
// Created by Ege Ozkan on 5.10.2020.
//


#include <string.h>
#include "fluxion_token.h"

/**
 * Initialise a base token given
 * @param token the newly allocated token.
 * @param lineCount line the token appears in.
 * @param tokenType type of the token.
 */
Token *initToken(int lineCount, TokenType tokenType) {
    Token *token = (Token*) malloc(sizeof(Token));
    token->lineCount = lineCount;
    token->tokenType = tokenType;
    return token;
}

/**
 * Free the base token, given
 * @param token Token to free.
 */
void freeBaseToken(Token *token) {
    free(token);
}

IdentifierToken *initIdentifierToken(int lineCount, const char *name) {
    IdentifierToken* token = (IdentifierToken*) malloc(sizeof(IdentifierToken));
    token->token = initToken(lineCount, IDENTIFIER);
    token->name = name; // You should probably copy this via strcpy.
    token->identifierType = Variable;
    return token;
}

void freeIdentifierToken(IdentifierToken *token) {
    freeBaseToken(token->token);
    free(token->name); // This will probably went terribly wrong, have you fixed line 24 yet?
    token->name = NULL;
    free(token);
    // Surely this will not segfault
}

FunctionToken *initFunctionToken(int lineCount, const char *name) {
    FunctionToken *token = malloc(sizeof(FunctionToken));
    token->identifier = initIdentifierToken(lineCount, name);
    token->current = 0;
    token->arity = 1;
    token->args = (Token **) malloc(token->arity * sizeof(Token*));
    return token;
}

void freeFunctionToken(FunctionToken *token) {
    freeIdentifierToken(token->identifier);
    free(token->args);
    token->args = NULL;
    free(token);
}

void addArgument(FunctionToken *token, Token *arg) {
    if (token->current >= token->arity) {
        token->arity *= 2;
        token->args = (Token **) realloc(token->args, sizeof(Token*) * token->arity);

    }
    token->args[token->current++] = arg;
}

void finaliseFunctionToken(FunctionToken *token) {
    if (token->current != token->arity) {
        token->arity = token->current; // Shrink to current size.
        token->args = realloc(token->args, sizeof(Token*) * token->arity);
    }
}

NumberToken *initNumberToken(int lineCount, double value) {
    NumberToken *token = (NumberToken*) malloc(sizeof(NumberToken));
    token->token = initToken(lineCount, NUMBER);
    token->value = value;
    return token;
}

void freeNumberToken(NumberToken *token) {
    freeBaseToken(token->token);
    free(token);
}

void mallocMatrixTokenArr(MatrixToken *matrix) {
    int arrSize = matrix->rowSize * matrix->columnSize;
    if (matrix->members == NULL) { // Not malloc'ed, yet.
        matrix->members = (Token **) malloc(sizeof(Token*) * arrSize);
    } else { // Enlarging an allocated array.
        matrix->members = (Token **) realloc(matrix->members, sizeof(Token *) * arrSize);
    }
}

MatrixToken *initMatrixToken(int lineCount) {
    MatrixToken  *token = (MatrixToken *) malloc(sizeof(Token));
    token->token = initToken(lineCount, MATRIX);
    token->columnSize = 0;
    token->rowSize = 0;
    token->members = NULL; // Means empty matrix
    return token;
}

void freeMatrixToken(MatrixToken *token) {
    free(token->members);
    token->members = NULL;
    freeBaseToken(token->token);
    token->token = NULL;
    free(token);
}

void matrixAddMember(MatrixToken *token, int row, int col, Token *element) {
    if ((row + 1) > token->rowSize || (col + 1) > token->columnSize) {
        token->rowSize = row ? row > token->rowSize : token->rowSize;
        token->columnSize = col ? col > token->columnSize : token->columnSize;
        mallocMatrixTokenArr(token);
    }
    token->members[row * col] = element; // Assign the value.
}

Token *matrixGetMember(MatrixToken *token, int row, int col) {
    return * (token->members + (row*col));
}

FiniteToken *initFiniteToken(int lineCount) {
    FiniteToken *token = (FiniteToken *) malloc(sizeof(FiniteToken));
    token->token = initToken(lineCount, FINITE);
    token->memberCount = 4;
    token->current = 0;
    token->members = (Token**) malloc(sizeof(Token*) * token->memberCount);
    return token;
}

void freeFiniteToken(FiniteToken *token) {
    freeBaseToken(token->token);
    token->token = NULL;
    free(token->members);
    token->members = NULL;
    free(token);
}

void finaliseFiniteToken(FiniteToken *token) {
    if (token->memberCount > token->current) {
        token->memberCount = token->current;
        token->members = (Token**) realloc(token->members, sizeof(Token*) * token->memberCount);
    }
}
void finiteAddElement(FiniteToken *token, Token *element) {
    if (token->current >= token->memberCount) {
        token->memberCount *= 2;
        token->members = (Token**) realloc(token->members, sizeof(Token*) * token->memberCount); // Double.
    }
    token->members[token->current++] = element;
}

OperatorToken *initOperatorToken(int lineCount, OperatorType operatorType) {
    OperatorToken *token = (OperatorToken*) malloc(sizeof(OperatorToken));
    token->token = initToken(lineCount, OPERATOR);
    token->operatorType = operatorType;
    return token;
}

void freeOperatorToken(OperatorToken *token) {
    freeBaseToken(token->token);
    token->token = NULL;
    free(token);
}

BuilderToken *initBuilderToken(int lineCount, IdentifierToken *variable, ExpressionToken *constraint) {
    BuilderToken *token = (BuilderToken*) malloc(sizeof(BuilderToken));
    token->token = initToken(lineCount, BUILDER);
    token->variable = variable;
    token->constraint = constraint;
    return token;
}

void freeBuilderToken(BuilderToken *token) {
    freeBaseToken(token->token);
    token->token = NULL;
    free(token);
}

SequenceToken *initSequenceToken(int lineCount, FiniteToken* prelist, IdentifierToken* variable, IdentifierToken* numerical, ExpressionToken* rule) {
    SequenceToken *token = (SequenceToken*) malloc(sizeof(SequenceToken));
    token->token = initToken(lineCount, SEQUENCE);
    token->prelist = prelist;
    token->variable = variable;
    token->numerical = numerical;
    token->rule = rule;
    return token;
}

void freeSequenceToken(SequenceToken *token) {
    freeBaseToken(token->token);
    free(token);
}

ExpressionToken *initExpressionToken(int lineCount) {
    ExpressionToken *token = (ExpressionToken *) malloc(sizeof(ExpressionToken));
    token->token = initToken(lineCount, EXPRESSION);
    token->tokenCount = 1;
    token->current = 0;
    token->tokens = (Token**) malloc(sizeof(Token*) * token->tokenCount);
    return token;
}

void freeExpressionToken(ExpressionToken *token) {
    free(token->tokens);
    token->tokens = NULL;
    freeBaseToken(token->token);
    token->token = NULL;
    free(token);
}

void ExpressionAddToken(ExpressionToken *token, Token *t) {
    if (token->current >= token->tokenCount) {
        token->tokenCount *= 2;
        token->tokens = (Token **) malloc(sizeof(Token*) * token->tokenCount);
    }
    token->tokens[token->current++] = t;
}

void finaliseExpressionToken(ExpressionToken *token) {
    if (token->tokenCount >= token->current) {
        token->tokenCount = token->current;
        token->tokens = (Token**) realloc(token->tokens, sizeof(Token*) * token->tokenCount);
    }
}

void freeToken(Token *token) {
    switch (token->tokenType) {
        case NUMBER:
            freeNumberToken((NumberToken *) token);
            break;
        case FINITE:
            freeFiniteToken((FiniteToken *) token);
            break;
        case BUILDER:
            freeBuilderToken((BuilderToken *) token);
            break;
        case MATRIX:
            freeMatrixToken((MatrixToken *) token);
            break;
        case SEQUENCE:
            freeSequenceToken((SequenceToken *) token);
            break;
        case EXPRESSION:
            freeExpressionToken((ExpressionToken *) token);
            break;
        case OPERATOR:
            freeOperatorToken((OperatorToken *) token);
            break;
        case IDENTIFIER:
            switch (((IdentifierToken*) token)->identifierType) {
                case Function:
                    freeFunctionToken((FunctionToken*) token);
                    break;
                case Variable:
                    freeIdentifierToken((IdentifierToken*) token);
                    break;
            }
            break;
    }
}


