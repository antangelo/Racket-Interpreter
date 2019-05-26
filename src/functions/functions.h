//
// Created by Antonio on 2019-01-09.
//

#ifndef RACKET_INTERPRETER_FUNCTIONS_H
#define RACKET_INTERPRETER_FUNCTIONS_H

#include <map>

#include "../expressions/expressions.h"

namespace Functions
{
    using Expressions::expression_vector;

    extern std::map<std::string, std::function<std::unique_ptr<Expressions::Expression>(expression_vector,
                                                                                        std::shared_ptr<Parser::Scope>)>> funcMap;

    extern std::map<std::string, std::function<std::unique_ptr<Expressions::Expression>(expression_vector,
                                                                                        std::shared_ptr<Parser::Scope>)>> specialFormMap;

    void registerFunctions();

    std::unique_ptr<Expressions::Expression> getFormByName(const std::string &, std::shared_ptr<Parser::Scope>);

    std::unique_ptr<Expressions::Expression> getFuncByName(const std::string &, std::shared_ptr<Parser::Scope>);
}

#endif //RACKET_INTERPRETER_FUNCTIONS_H
