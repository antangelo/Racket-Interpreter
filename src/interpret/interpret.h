//
// Created by Antonio Abbatangelo on 2019-06-02.
//

#ifndef RACKET_INTERPRETER_INTERPRET_H
#define RACKET_INTERPRETER_INTERPRET_H

#include "parser.h"

namespace Interpreter
{
    std::unique_ptr<Expressions::Expression> interpret(std::unique_ptr<Expressions::Expression>);

    Expressions::expression_vector interpretSaveSteps(std::unique_ptr<Expressions::Expression>);
}

#endif //RACKET_INTERPRETER_INTERPRET_H
