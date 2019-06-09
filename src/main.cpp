#include <iostream>

#include "interpret/interpret.h"
#include "functions/functions.h"

void repl(std::shared_ptr<Expressions::Scope> &globalScope);

int main(int argc, char *argv[])
{
    std::cout << "Run '(exit)' to exit." << std::endl;

    Functions::registerFunctions();

    std::shared_ptr<Expressions::Scope> globalScope(new Expressions::Scope(nullptr));

    repl(globalScope);

    globalScope->clear();

    return 0;
}