#include <iostream>

#include "parser.h"
#include "functions.h"

int main(int argc, char *argv[])
{
    std::cout << "Racket Interpreter Alpha 1" << std::endl;
    std::cout << "Run '(exit)' to exit." << std::endl;

    Functions::registerFunctions();
    bool hideSteps = true;

    for (;;)
    {
        try
        {

            std::cout << "> ";
            std::string input;
            std::getline(std::cin, input);

            if (input == "(exit)") break;
            if (input == "(toggle-step)") hideSteps = !hideSteps;
            else
            {
                std::unique_ptr<Expressions::Expression> expr;
                auto parseSuccess = Parser::parse(input, expr);

                if (!parseSuccess)
                {
                    std::cout << "Invalid input: " + input << std::endl;
                    continue;
                }

                auto exprPtr = expr.get();
                int steps = 0;

                while (!exprPtr->isValue())
                {
                    if (steps >= 100) break;

                    expr = exprPtr->evaluate(&expr);
                    exprPtr = expr.get();

                    if (dynamic_cast<Expressions::VoidValueExpression *>(exprPtr))
                    {
                        continue; // Don't print void values on their own
                    }

                    if (hideSteps) continue;

                    std::cout << "Step " << steps++ << std::endl;
                    std::cout << *exprPtr << std::endl;
                }

                // Prints output if stepping is off, since the final statement won't be printed otherwise
                if (hideSteps && dynamic_cast<Expressions::VoidValueExpression *> (exprPtr) == nullptr)
                    std::cout << *exprPtr << std::endl;
            }
        }
        catch (std::exception &exception)
        {
            std::cout << "Exception occurred: " << exception.what() << std::endl;
        }
    }

    return 0;
}