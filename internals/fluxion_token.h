//
// Created by Ege Ozkan on 5.10.2020.
//

#ifndef FLUXIONCORE_FLUXION_TOKEN_H
#define FLUXIONCORE_FLUXION_TOKEN_H

#include "commons.h"

/**
 * Enum for token type.
 */
typedef enum {
    NUMBER,
    FINITE,
    BUILDER,
    RULE,
    MATRIX,
    SEQUENCE,
    EXPRESSION,
    OPERATION,
    IDENTIFIER
} TokenType;

/**
 * General, base struct for all tokens.
 */
typedef struct {
    int lineCount;
    TokenType tokenType;
} Token;




/**
 * Initialise a token given
 * @param token the newly allocated token.
 * @param lineCount line the token appears in.
 * @param tokenType type of the token.
 */
Token *initToken(int lineCount, TokenType tokenType);
void freeToken(Token *token);

/**
 * Represent any identifier.
 */
typedef struct {
    Token *token;
    const char *name;
    IdentifierType identifierType;
} IdentifierToken;

/**
 * Initialise an identifier.
 * @param token Token to init.
 * @param name Name of the identifier.
 */
IdentifierToken *initIdentifierToken(int lineCount, const char *name);

/**
 * Free the identifier.
 * @param token
 */
void freeIdentifierToken(IdentifierToken *token);

/**
 * Represent any Function
 */
typedef struct {
    IdentifierToken *identifier;
    Token **args;
    int current;
    int arity;
} FunctionToken;

FunctionToken *initFunctionToken(int lineCount, const char *name);
void freeFunctionToken(FunctionToken *token);
void addArgument(FunctionToken *token, Token *arg);
void finaliseFunctionToken(FunctionToken *token);

/**
 * A struct to hold numbers.
 */
typedef struct {
    Token *token;
    double value;
} NumberToken;

/**
 * Initialise a number token, given.
 * @param token the newly allocated pointer to the token.
 * @param lineCount the line the token appears in.
 * @param value Value of the transaction.
 */
NumberToken *initNumberToken(int lineCount, double value);
/**
 * Free the memory allocated to number token.
 * @param token Token to free.
 */
void freeNumberToken(NumberToken *token);

/**
 * Represents a matrix. Dynamically allocated.
 */
typedef struct {
    Token *token;
    Token** members; // Flattened for higher performance.
    int rowSize;
    int columnSize;
} MatrixToken;

/**
 * Initialise a matrix token.
 * @param token to initialise.
 * @param lineCount the token appears in.
 */
MatrixToken *initMatrixToken(int lineCount);
/**
 * Free the matrix token.
 * @param token Token to free.
 */
void freeMatrixToken(MatrixToken *token);
/**
 * Add an element to the matrix.
 * @param token Matrix to add to.
 * @param row Row to add the element to.
 * @param col Col to add the element to.
 * @param element Element to add.
 */
void matrixAddMember(MatrixToken *token, int row, int col, Token *element);
Token *matrixGetMember(MatrixToken *token, int row, int col);

/**
 * Represents a sequence castable enumerable.
 * Is dynamically allocated.
 */
typedef struct {
    Token **members;
    int memberCount;
    int current;
} FiniteToken;

/**
 * Initialise the token.
 * @param token Token to initialise.
 */
void initFiniteToken(FiniteToken *token);
/**
 * Free the previously initialised token.
 * @param token to free.
 */
void freeFiniteToken(FiniteToken *token);
/**
 * Finalise the token by finalising its member count.
 * ie: Trimming empty space.
 * @param token Token to finalise.
 */
void finaliseFiniteToken(FiniteToken *token);
/**
 * Add an element to the collection.
 * @param token Token to add into.
 * @param element element to add to.
 */
void FiniteAddElement(FiniteToken *token, Token *element);

/**
 * Represents an expression.
 */
typedef struct {
    Token *token;
    Token **tokens;
    int tokenCount;
} ExpressionToken;

void initExpressionToken(ExpressionToken *token, int tokenCount);
void freeExpressionToken(ExpressionToken *token);


#endif //FLUXIONCORE_FLUXION_TOKEN_H
