#include "args/args.h"
#include "interpret/interpret.h"
#include "functions/functions.h"

int main(int argc, char *argv[])
{
    Functions::registerFunctions();
    std::shared_ptr<Expressions::Scope> globalScope(new Expressions::Scope(nullptr));

    globalScope->define("e", std::make_unique<Expressions::InexactNumberExpression>
            (Expressions::InexactNumberExpression(M_E, globalScope)));
    globalScope->define("pi", std::make_unique<Expressions::InexactNumberExpression>
            (Expressions::InexactNumberExpression(M_PI, globalScope)));

    CLI::parseCmdArgs(argc, argv, globalScope);

    std::cout << "Run '(exit)' to exit." << std::endl;

    Interpreter::repl(std::cin, globalScope, false);

    globalScope->clear();
    return 0;
}