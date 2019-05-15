#include <iostream>

#include "parser.h"
#include "functions/functions.h"

int main(int argc, char *argv[])
{
    std::cout << "Racket Interpreter Alpha 1" << std::endl;
    std::cout << "Run '(exit)' to exit." << std::endl;

    Functions::registerFunctions();
    bool hideSteps = true;

    std::unique_ptr<Parser::Scope> globalScope(new Parser::Scope());

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
                auto parseSuccess = Parser::parse(input, globalScope.get(), expr);

                if (!parseSuccess)
                {
                    std::cout << "Invalid input: " + input << std::endl;
                    continue;
                }

                int steps = 0;

                while (!expr->isValue())
                {
                    if (steps >= 100) break;

                    expr = Expressions::evaluate(std::move(expr), globalScope.get());

                    if (dynamic_cast<Expressions::VoidValueExpression *>(expr.get()))
                    {
                        continue; // Don't print void values on their own
                    }

                    if (hideSteps) continue;

                    std::cout << "Step " << steps++ << std::endl;
                    std::cout << *expr << std::endl;
                }

                // Prints output if stepping is off, since the final statement won't be printed otherwise
                if (hideSteps && dynamic_cast<Expressions::VoidValueExpression *> (expr.get()) == nullptr)
                    std::cout << *expr << std::endl;
            }
        }
        catch (std::exception &exception)
        {
            std::cout << "Exception occurred: " << exception.what() << std::endl;
            //throw;
        }
    }

    return 0;
}