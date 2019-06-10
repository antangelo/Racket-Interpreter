//
// Created by Antonio Abbatangelo on 2019-06-09.
//

#include "interpret/interpret.h"

std::string read()
{
    std::cout << "> ";
    std::string out, input;
    std::getline(std::cin, input);
    out += input;

    while (std::cin && Parser::findTupleEnd(out) == std::string::npos)
    {
        out += "\n";
        std::getline(std::cin, input);
        out += input;
    }

    return out;
}

std::unique_ptr<Expressions::Expression> eval(const std::string &input,
                                              std::shared_ptr<Expressions::Scope> &globalScope)
{
    auto expr = Parser::parse(input, globalScope);
    return Interpreter::interpret(std::move(expr));
}

Expressions::expression_vector evalSteps(const std::string &input,
                                         std::shared_ptr<Expressions::Scope> &globalScope)
{
    auto expr = Parser::parse(input, globalScope);
    return Interpreter::interpretSaveSteps(std::move(expr));
}

void print(std::unique_ptr<Expressions::Expression> &expr)
{
    if (dynamic_cast<Expressions::VoidValueExpression *>(expr.get())) return;

    std::cout << expr->toString() << std::endl;
}

void print(Expressions::expression_vector &steps)
{
    for (auto &exp : steps)
    {
        if (dynamic_cast<Expressions::VoidValueExpression *>(exp.get()) ||
            dynamic_cast<Expressions::PartialExpression *>(exp.get()))
            continue;

        std::cout << exp->toString() << std::endl;
    }
}

void repl(std::shared_ptr<Expressions::Scope> &globalScope)
{
    bool hideSteps = true;

    while (std::cin)
    {
        try
        {
            std::string input = read();
            if (input.empty())
            {
                if (!std::cin) std::cout << std::endl;
                continue;
            }

            if (input == "(exit)") break;
            if (input == "(toggle-step)") hideSteps = !hideSteps;
            else if (hideSteps)
            {
                auto expr = eval(input, globalScope);
                print(expr);
            }
            else
            {
                auto steps = evalSteps(input, globalScope);
                print(steps);
            }
        }
        catch (std::exception &exception)
        {
            std::cout << exception.what() << std::endl;
        }
    }
}