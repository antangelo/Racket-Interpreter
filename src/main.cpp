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
            auto expr = Parser::parse(input);

            for(auto& i : expr)
            {
                std::cout << i << std::endl;
            }
        }
    }

    return 0;
}