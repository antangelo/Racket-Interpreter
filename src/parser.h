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
        explicit Scope(std::shared_ptr<Scope> parent)
        {
            this->parent = std::move(parent);

            if (this->parent && this->parent->globalScope) this->globalScope = this->parent->globalScope;
            else this->globalScope = this->parent.get();
        }

        bool contains(const std::string &);

        void define(const std::string &, std::unique_ptr<Expressions::Expression>);

        void defineGlobal(const std::string &, std::unique_ptr<Expressions::Expression>);

        std::unique_ptr<Expressions::Expression> getDefinition(const std::string &);

        std::string toString();

    public:
        std::map<std::string, std::unique_ptr<Expressions::Expression>> definitions; //TODO: replace string
        Scope *globalScope;

    private:
        std::shared_ptr<Scope> parent;
    };

    std::vector<std::string> parseTuple(std::string);

    void replaceInScope(std::string &, const std::string &, const std::string &);

    void
    parseSpecialForm(const std::string &, const std::shared_ptr<Scope> &, std::unique_ptr<Expressions::Expression> &);

    bool parse(std::string, const std::shared_ptr<Scope> &, std::unique_ptr<Expressions::Expression> &);
}

#endif //RACKET_INTERPRETER_PARSER_H
