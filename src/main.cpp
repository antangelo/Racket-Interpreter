#include "args/args.h"
#include "interpret/interpret.h"
#include "functions/functions.h"
#include "expressions/struct_expression.h"

void define_constants(std::shared_ptr<Expressions::Scope> &globalScope)
{
    globalScope->define("e", std::make_unique<Expressions::InexactNumberExpression>
            (Expressions::InexactNumberExpression(M_E, globalScope)));
    globalScope->define("pi", std::make_unique<Expressions::InexactNumberExpression>
            (Expressions::InexactNumberExpression(M_PI, globalScope)));

    StructFunctions::defineStruct("posn", std::vector<std::string>{"x", "y"});
}

int main(int argc, char *argv[])
{
    Functions::registerFunctions();
    std::shared_ptr<Expressions::Scope> globalScope(new Expressions::Scope(nullptr));
    define_constants(globalScope);

    CLI::parseCmdArgs(argc, argv, globalScope);

    std::cout << "Run '(exit)' to exit." << std::endl;

    Interpreter::repl(std::cin, globalScope, false);

    globalScope->clear();
    return 0;
}