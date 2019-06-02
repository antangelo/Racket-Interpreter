//
// Created by antonio on 27/12/18.
//

#ifndef RACKET_INTERPRETER_PARSER_H
#define RACKET_INTERPRETER_PARSER_H

#include <string>
#include <list>
#include <functional>

#include "../expressions/expressions.h"

namespace Parser
{
    std::vector<std::string> parseTuple(std::string);

    void replaceInScope(std::string &, const std::string &, const std::string &);

    void
    parseSpecialForm(const std::string &, const std::shared_ptr<Expressions::Scope> &,
                     std::unique_ptr<Expressions::Expression> &);

    std::unique_ptr<Expressions::Expression> parse(std::string, const std::shared_ptr<Expressions::Scope> &);
}

#endif //RACKET_INTERPRETER_PARSER_H
