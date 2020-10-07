//
// Created by Ege Ozkan on 5.10.2020.
//


#include <string.h>
#include "fluxion_token.h"

Token *initToken(int lineCount, TokenType tokenType) {
    Token *token = (Token*) malloc(sizeof(Token));
    token->lineCount = lineCount;
    token->tokenType = tokenType;
    return token;
}

void freeToken(Token *token) {
    free(token);
    token = NULL;
}

IdentifierToken *initIdentifierToken(int lineCount, const char *name) {
    IdentifierToken* token = (IdentifierToken*) malloc(sizeof(IdentifierToken));
    token->token = initToken(lineCount, IDENTIFIER);
    token->name = name; // You should probably copy this via strcpy.
    token->identifierType = Variable;
    return token;
}

void freeIdentifierToken(IdentifierToken *token) {
    freeToken(token->token);
    free(token->name); // This will probably went terribly wrong, have you fixed line 24 yet?
    token->name = NULL;
    free(token);
    token = NULL;
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
    token = NULL;
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
    freeToken(token->token);
    free(token);
    token = NULL;
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
    freeToken(token->token);
    token->token = NULL;
    free(token);
    token = NULL;
}

void matrixAddMember(MatrixToken *token, int row, int col, Token *element) {
    if ((row + 1) > token->rowSize || (col + 1) > token->columnSize) {
        token->rowSize = row ? row > token->rowSize : token->rowSize;
        token->columnSize = col ? col > token->columnSize : token->columnSize;
        mallocMatrixTokenArr(token);
    }
    * (token->members + (row*col)) = element; // Assign the value.
}

Token *matrixGetMember(MatrixToken *token, int row, int col) {
    return * (token->members + (row*col));
}



