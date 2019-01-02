//
// Created by antonio on 27/12/18.
//

#ifndef RACKET_INTERPRETER_PARSER_H
#define RACKET_INTERPRETER_PARSER_H
#include <string>
#include <vector>

class Expression //TODO: Namespace for Expressions
{
public:
    /* Should return true if the expression is a value, that is,
     * if it can't be simplified any further. */
    virtual bool isValue() = 0;

    /* Should evaluate the expression and produce an equivalent expression that is
     * one step closer to being a value. If the expression is a value, should produce itself. */
    virtual Expression& evaluate() = 0;

protected:
    explicit Expression() = default;
};

namespace Parser
{

    std::vector<std::string> parse(std::string);

}

#endif //RACKET_INTERPRETER_PARSER_H
