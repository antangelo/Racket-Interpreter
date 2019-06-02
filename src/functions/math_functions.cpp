//
// Created by Antonio Abbatangelo on 2019-05-26.
//

#include "functions.h"
#include "../interpret/parser.h"

typedef std::unique_ptr<Expressions::Expression> expr_ptr;
typedef std::shared_ptr<Expressions::Scope> scope_ptr;
using Expressions::expression_vector;

expr_ptr plus_func(expression_vector expr, scope_ptr scope)
{
    boost::rational<int> sum = 0;

    for (auto &i : expr)
    {
        if (auto num = dynamic_cast<Expressions::NumericalValueExpression *>(i.get()))
        {
            sum += num->mValue;
        }
        else throw std::invalid_argument("+ expects numerical arg, found: " + i->toString());
    }

    return std::unique_ptr<Expressions::Expression>
            (new Expressions::NumericalValueExpression(sum,
                                                       std::make_unique<Expressions::Scope>(
                                                               Expressions::Scope(
                                                                       std::move(scope)))));
}

expr_ptr sub_func(expression_vector expr, scope_ptr scope)
{
    if (expr.empty()) throw std::invalid_argument("- expected at least 1 arg.");

    if (auto first = dynamic_cast<Expressions::NumericalValueExpression *>(expr.front().get()))
    {
        if (expr.size() == 1)
        {
            return std::unique_ptr<Expressions::Expression>
                    (new Expressions::NumericalValueExpression(-1 * first->mValue,
                                                               std::make_unique<Expressions::Scope>(
                                                                       Expressions::Scope(std::move(scope)))));
        }

        boost::rational<int> diff = first->mValue;

        for (int i = 1; i < expr.size(); i++)
        {
            auto &j = expr[i];
            if (auto num = dynamic_cast<Expressions::NumericalValueExpression *>(j.get()))
            {
                diff -= num->mValue;
            }
            else throw std::invalid_argument("- expects numerical arg, found: " + j->toString());
        }

        return std::unique_ptr<Expressions::Expression>
                (new Expressions::NumericalValueExpression(diff,
                                                           std::make_unique<Expressions::Scope>(
                                                                   Expressions::Scope(
                                                                           std::move(scope)))));
    }
    else throw std::invalid_argument("- expects numerical arg, found: " + expr.front()->toString());
}

expr_ptr mult_func(expression_vector expr, scope_ptr scope)
{
    boost::rational<int> product(1);

    for (auto &i : expr)
    {
        if (auto num = dynamic_cast<Expressions::NumericalValueExpression *>(i.get()))
        {
            product *= num->mValue;
        }
        else throw std::invalid_argument("* expects numerical arg, found: " + i->toString());
    }

    return std::unique_ptr<Expressions::Expression>
            (new Expressions::NumericalValueExpression(product,
                                                       std::make_unique<Expressions::Scope>(
                                                               Expressions::Scope(
                                                                       std::move(scope)))));
}

expr_ptr div_func(expression_vector expr, scope_ptr scope)
{
    if (expr.empty()) throw std::invalid_argument("/ expected at least 1 arg.");

    if (auto first = dynamic_cast<Expressions::NumericalValueExpression *>(expr.front().get()))
    {
        if (expr.size() == 1)
        {
            return std::unique_ptr<Expressions::Expression>
                    (new Expressions::NumericalValueExpression(1 / first->mValue,
                                                               std::make_unique<Expressions::Scope>(
                                                                       Expressions::Scope(std::move(scope)))));
        }

        boost::rational<int> quotient = first->mValue;

        for (int i = 1; i < expr.size(); i++)
        {
            auto &j = expr[i];
            if (auto num = dynamic_cast<Expressions::NumericalValueExpression *>(j.get()))
            {
                quotient /= num->mValue;
            }
            else throw std::invalid_argument("/ expects numerical arg, found: " + j->toString());
        }

        return std::unique_ptr<Expressions::Expression>(new Expressions::NumericalValueExpression(quotient,
                                                                                                  std::make_unique<Expressions::Scope>(
                                                                                                          Expressions::Scope(
                                                                                                                  std::move(
                                                                                                                          scope)))));
    }
    else throw std::invalid_argument("/ expects numerical arg, found: " + expr.front()->toString());
}

void register_math_functions()
{
    Functions::funcMap["+"] = plus_func;
    Functions::funcMap["-"] = sub_func;
    Functions::funcMap["*"] = mult_func;
    Functions::funcMap["/"] = div_func;
}

