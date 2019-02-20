//
// Created by Antonio on 2019-01-09.
//

#ifndef RACKET_INTERPRETER_FUNCTIONS_H
#define RACKET_INTERPRETER_FUNCTIONS_H

#include <map>

#include "expressions.h"

namespace Functions
{
    using Expressions::expression_vector;

    extern std::map<std::string, std::function<std::unique_ptr<Expressions::Expression>(expression_vector,
                                                                                        Parser::Scope *)>> funcMap;

    void registerFunctions();

    std::unique_ptr<Expressions::Expression> getFuncByName(std::string);
}

#endif //RACKET_INTERPRETER_FUNCTIONS_H
