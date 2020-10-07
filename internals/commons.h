//
// Created by Ege Ozkan on 5.10.2020.
//

#ifndef FLUXIONCORE_COMMONS_H
#define FLUXIONCORE_COMMONS_H
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
typedef enum {
    Function,
    Variable
} IdentifierType;

typedef enum {
    // SCOPE
    SCOPE,
    ASSIGN,
    // BOOLEAN
    LESS,
    GREATER,
    LEQ,
    GEQ,
    EQUAL,
    NEQ,
    // MULTI-USE
    AMPERSAND,
    BAR,
    NOT,
    // DIFFERENTIAL
    DIFF,
    LIMIT,
    // SETS, SEQUENCES
    IN,
    GET,
    // ALGEBRAIC
    PLUS,
    MINUS,
    MULTIPLY,
    POWER,
    FACTORIAL
} OperatorType;

#endif //FLUXIONCORE_COMMONS_H
