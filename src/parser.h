//
// Created by antonio on 27/12/18.
//

#ifndef RACKET_INTERPRETER_PARSER_H
#define RACKET_INTERPRETER_PARSER_H

#include <string>
#include <list>
#include <functional>
#include <map>

#include "expressions/expressions.h"

namespace Parser
{
    class Scope
    {
    public:
        Scope()
        {
            mParent = nullptr;
        }

        explicit Scope(Scope *parent)
        {
            mParent = parent;
        }

        bool contains(const std::string &);

        void define(std::string, std::unique_ptr<Expressions::Expression>);

        std::unique_ptr<Expressions::Expression> getDefinition(const std::string &);

    public:
        std::map<std::string, std::unique_ptr<Expressions::Expression>> definitions; //TODO: replace string

    private:
        Scope *mParent;
    };

    std::vector<std::string> parseTuple(std::string);

    void replaceInScope(std::string &, std::string, std::string);

    void parseSpecialForm(std::string, Scope *, std::unique_ptr<Expressions::Expression> &);

    bool parse(std::string, Scope *, std::unique_ptr<Expressions::Expression> &);
}

#endif //RACKET_INTERPRETER_PARSER_H
