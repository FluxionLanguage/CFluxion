//
// Created by Ege Ozkan on 5.10.2020.
//

#include <stdio.h>
#include "commons.h"

const char *getLiteralName (ErrorLiteral literal) {
    switch (literal) {
        case Undefined:
            return "Undefined";
        case Indeterminate:
            return "Indeterminate";
        case Overflow:
            return "Overflow";
    }
}

void issueError(Error *error) {
    fprintf(stderr, "%s: %s", getLiteralName(error->errorLiteral), error->errorMessage);
}