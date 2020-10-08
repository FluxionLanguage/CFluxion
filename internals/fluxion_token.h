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
    MATRIX,
    SEQUENCE,
    EXPRESSION,
    OPERATOR,
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
 * Represent any identifier.
 */
typedef struct {
    Token *token;
    char *name;
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

/**
 * Initialise the function token, given
 * @param lineCount Line the token appeared in.
 * @param name Name of the identifier.
 * @return the pointer to the newly created function
 */
FunctionToken *initFunctionToken(int lineCount, const char *name);
/**
 * Free the function token.
 * @param token Token to free.
 */
void freeFunctionToken(FunctionToken *token);
/**
 * Add an argument to the function.
 * @param token Pointer to function token.
 * @param arg Argument to add.
 */
void addArgument(FunctionToken *token, Token *arg);
/**
 * Finalise the function token. By trimming it.
 * @param token Token to finalise
 */
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
    Token *token;
    Token **members;
    int memberCount;
    int current;
} FiniteToken;

/**
 * Initialise the token.
 * @param lineCount The line the token was in.
 */
FiniteToken *initFiniteToken(int lineCount);
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
void finiteAddElement(FiniteToken *token, Token *element);

/**
 * A typedef that holds operators.
 */
typedef struct {
    Token *token;
    OperatorType operatorType;
} OperatorToken;

/**
 * Initialise a new operator token.
 * @param lineCount Line the operator appears in.
 * @param operatorType Type of the operator.
 * @return Pointer to the newly created operator token.
 */
OperatorToken *initOperatorToken(int lineCount, OperatorType operatorType);
/**
 * Free the operator token.
 * @param token
 */
void freeOperatorToken(OperatorToken *token);

/**
 * Represents an expression.
 */
typedef struct {
    Token *token;
    Token **tokens;
    int current;
    int tokenCount;
} ExpressionToken;

ExpressionToken *initExpressionToken(int lineCount);
void freeExpressionToken(ExpressionToken *token);
void ExpressionAddToken(ExpressionToken *token, Token *t);
void finaliseExpressionToken(ExpressionToken *token);

/**
 * A typedef that represents a
 * set definition using the set
 * builder notation.
 */
typedef struct {
    Token *token;
    IdentifierToken *variable; // Part before the |
    ExpressionToken *constraint; // Part after the |
} BuilderToken;

BuilderToken *initBuilderToken(int lineCount, IdentifierToken *variable, ExpressionToken *constraint);
void freeBuilderToken(BuilderToken *token);

/**
 * The sequence rule declarations
 * of the form
 * {1, 1} x_n -> x_{n - 1} + x_{n - 2}
 */
typedef struct {
    Token *token;
    FiniteToken *prelist; // The before part.
    IdentifierToken *variable; // x of the x_n
    IdentifierToken *numerical; //n of the x_n
    ExpressionToken  *rule; // The actual generation rule.
} SequenceToken;

SequenceToken *initSequenceToken(int lineCount, FiniteToken* prelist, IdentifierToken* variable, IdentifierToken* numerical, ExpressionToken* rule);
void freeSequenceToken(SequenceToken *token);

/**
 * Free a token, with respect to its Token type.
 * @param token Token to free.
 */
void freeToken(Token *token);

#endif //FLUXIONCORE_FLUXION_TOKEN_H
