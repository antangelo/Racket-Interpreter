//
// Created by Antonio Abbatangelo on 2019-06-02.
//

#include "interpret.h"

namespace Interpreter
{
    std::unique_ptr<Expressions::Expression> interpret(std::unique_ptr<Expressions::Expression> expr)
    {
        while (!expr->isValue())
        {
            expr = Expressions::evaluate(std::move(expr));
        }

        return expr;
    }

    Expressions::expression_vector interpretSaveSteps(std::unique_ptr<Expressions::Expression> expr)
    {
        Expressions::expression_vector vec;

        while (!expr->isValue())
        {
            vec.push_back(expr->clone());

            try
            {
                expr = Expressions::evaluate(std::move(expr));
            }
            catch (std::exception &e)
            {
                std::cerr << e.what() << std::endl;

                return vec;
            }
        }

        vec.push_back(std::move(expr));
        return vec;
    }
}