//
// Created by antonio on 27/12/18.
//

#ifndef RACKET_INTERPRETER_PARSER_H
#define RACKET_INTERPRETER_PARSER_H

#include <string>
#include <list>
#include <functional>

#include "expressions.h"

namespace Parser
{
    std::vector<std::string> parseTuple(std::string);

    void replaceInScope(std::string &, std::string, std::string);

    bool parse(std::string, std::unique_ptr<Expressions::Expression> &);
}

#endif //RACKET_INTERPRETER_PARSER_H
