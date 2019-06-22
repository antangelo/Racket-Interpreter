//
// Created by Antonio Abbatangelo on 2019-06-22.
//

#ifndef RACKET_INTERPRETER_STRUCT_EXPRESSION_H
#define RACKET_INTERPRETER_STRUCT_EXPRESSION_H

#include <string>
#include <vector>

namespace StructFunctions
{
    void defineStruct(const std::string &structName, const std::vector<std::string> &structFields);
}

#endif //RACKET_INTERPRETER_STRUCT_EXPRESSION_H
