//
// Created by Antonio on 2019-01-09.
//

#ifndef RACKET_INTERPRETER_FUNCTIONS_H
#define RACKET_INTERPRETER_FUNCTIONS_H

#include "../expressions/expressions.h"

namespace Functions
{
    using Expressions::expression_vector;

    extern std::map<std::string, std::function<std::unique_ptr<Expressions::Expression>(expression_vector,
                                                                                        std::shared_ptr<Expressions::Scope>)>> funcMap;

    extern std::map<std::string, std::function<std::unique_ptr<Expressions::Expression>(expression_vector,
                                                                                        std::shared_ptr<Expressions::Scope>)>> specialFormMap;

    void registerFunctions();

    std::unique_ptr<Expressions::Expression> getFormByName(const std::string &, std::shared_ptr<Expressions::Scope>);

    std::unique_ptr<Expressions::Expression> getFuncByName(const std::string &, std::shared_ptr<Expressions::Scope>);
}

#endif //RACKET_INTERPRETER_FUNCTIONS_H
