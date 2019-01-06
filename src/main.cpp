#include <iostream>

#include "parser.h"

int main(int argc, char *argv[])
{
    std::cout << "Racket Interpreter Alpha 1" << std::endl;
    std::cout << "Run '(exit)' to exit." << std::endl;

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

            if (parseSuccess) std::cout << *(exprPtr->evaluate(&expr)) << std::endl;
        }
    }

    return 0;
}