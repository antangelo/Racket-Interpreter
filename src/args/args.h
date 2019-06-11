//
// Created by Antonio Abbatangelo on 2019-06-10.
//

#ifndef RACKET_INTERPRETER_ARGS_H
#define RACKET_INTERPRETER_ARGS_H

#include "../expressions/expressions.h"
#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"

namespace CLI
{
    void parseCmdArgs(int argc, char *argv[], std::shared_ptr<Expressions::Scope> &);
}

#endif //RACKET_INTERPRETER_ARGS_H
