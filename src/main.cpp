#include <iostream>

#include "parser.h"
#include "functions.h"

int main(int argc, char *argv[])
{
    std::cout << "Racket Interpreter Alpha 1" << std::endl;
    std::cout << "Run '(exit)' to exit." << std::endl;

    Functions::registerFunctions();

    for (;;)
    {
        std::cout << "> ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "(exit)") break;
        else
        {
            std::unique_ptr<Expressions::Expression> expr;
            auto parseSuccess = Parser::parse(input, expr);
            auto exprPtr = expr.get();
            int steps = 0;

            while (!exprPtr->isValue())
            {
                if (steps >= 100) break;

                std::cout << "Step " << steps++ << std::endl;
                std::cout << *exprPtr << std::endl;

                expr = exprPtr->evaluate(&expr);
                exprPtr = expr.get();
            }

            std::cout << "Step " << steps << std::endl;
            std::cout << *exprPtr << std::endl;
        }
    }

    return 0;
}