//
// Created by Antonio on 2019-01-09.
//

#include "functions.h"

namespace Functions
{
    std::map<std::string, std::function<std::unique_ptr<Expressions::Expression>(expression_vector)>> funcMap;

    void registerFunctions()
    {
        funcMap["+"] = [](expression_vector expr) -> std::unique_ptr<Expressions::Expression>
        {
            double sum = 0;

            for (auto &i : expr)
            {
                if (auto num = dynamic_cast<Expressions::NumericalValueExpression *>(i.get()))
                {
                    sum += num->mValue;
                } else throw std::invalid_argument("+ expects numerical arg, found: " + i->toString());
            }

            return std::unique_ptr<Expressions::Expression>(new Expressions::NumericalValueExpression(sum));
        };

        funcMap["*"] = [](expression_vector expr) -> std::unique_ptr<Expressions::Expression>
        {
            double product = 1;

            for (auto &i : expr)
            {
                if (auto num = dynamic_cast<Expressions::NumericalValueExpression *>(i.get()))
                {
                    product *= num->mValue;
                } else throw std::invalid_argument("* expects numerical arg, found: " + i->toString());
            }

            return std::unique_ptr<Expressions::Expression>(new Expressions::NumericalValueExpression(product));
        };

        funcMap["-"] = [](expression_vector expr) -> std::unique_ptr<Expressions::Expression>
        {
            if (expr.empty()) throw std::invalid_argument("- expected at least 1 arg.");

            if (auto first = dynamic_cast<Expressions::NumericalValueExpression *>(expr.front().get()))
            {
                if (expr.size() == 1)
                {
                    return std::unique_ptr<Expressions::Expression>
                            (new Expressions::NumericalValueExpression(-1 * first->mValue));
                }

                double diff = first->mValue;

                for (int i = 1; i < expr.size(); i++)
                {
                    auto &j = expr[i];
                    if (auto num = dynamic_cast<Expressions::NumericalValueExpression *>(j.get()))
                    {
                        diff -= num->mValue;
                    } else throw std::invalid_argument("- expects numerical arg, found: " + j->toString());
                }

                return std::unique_ptr<Expressions::Expression>(new Expressions::NumericalValueExpression(diff));
            } else throw std::invalid_argument("- expects numerical arg, found: " + expr.front()->toString());
        };

        funcMap["/"] = [](expression_vector expr) -> std::unique_ptr<Expressions::Expression>
        {
            if (expr.empty()) throw std::invalid_argument("/ expected at least 1 arg.");

            if (auto first = dynamic_cast<Expressions::NumericalValueExpression *>(expr.front().get()))
            {
                if (expr.size() == 1)
                {
                    return std::unique_ptr<Expressions::Expression>
                            (new Expressions::NumericalValueExpression(1 / first->mValue));
                }

                double quotient = first->mValue;

                for (int i = 1; i < expr.size(); i++)
                {
                    auto &j = expr[i];
                    if (auto num = dynamic_cast<Expressions::NumericalValueExpression *>(j.get()))
                    {
                        quotient /= num->mValue;
                    } else throw std::invalid_argument("/ expects numerical arg, found: " + j->toString());
                }

                return std::unique_ptr<Expressions::Expression>(new Expressions::NumericalValueExpression(quotient));
            } else throw std::invalid_argument("/ expects numerical arg, found: " + expr.front()->toString());
        };
    }

    std::unique_ptr<Expressions::Expression> getFuncByName(std::string name)
    {
        auto m = funcMap.at(name);

        return std::unique_ptr<Expressions::Expression>(new Expressions::FunctionExpression(name, m));
    }
}