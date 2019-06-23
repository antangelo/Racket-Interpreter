//
// Created by Antonio Abbatangelo on 2019-05-26.
//

#include "functions.h"
#include "../interpret/parser.h"
#include <cmath>

typedef std::unique_ptr<Expressions::Expression> expr_ptr;
typedef std::shared_ptr<Expressions::Scope> scope_ptr;
using Expressions::expression_vector;

expr_ptr plus_func(expression_vector expr, scope_ptr scope)
{
    boost::rational<int> sum = 0;
    double dSum = 0.0;
    bool returnFloating = false;

    for (auto &i : expr)
    {
        if (auto num = dynamic_cast<Expressions::NumericalValueExpression *>(i.get()))
        {
            sum += num->mValue;
        }
        else if (auto fl = dynamic_cast<Expressions::InexactNumberExpression *>(i.get()))
        {
            returnFloating = true;
            dSum += fl->value;
        }
        else throw std::invalid_argument("+ expects numerical arg, found: " + i->toString());
    }

    if (returnFloating)
    {
        return std::make_unique<Expressions::InexactNumberExpression>
                (Expressions::InexactNumberExpression(dSum + boost::rational_cast<double>(sum),
                                                      std::move(scope)));
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

    boost::rational<int> diff = 0;
    double dDiff = 0.0;
    bool returnFloating = false;

    if (auto first = dynamic_cast<Expressions::NumericalValueExpression *>(expr.front().get()))
    {
        if (expr.size() == 1)
        {
            return std::unique_ptr<Expressions::Expression>
                    (new Expressions::NumericalValueExpression(-1 * first->mValue,
                                                               std::make_unique<Expressions::Scope>(
                                                                       Expressions::Scope(std::move(scope)))));
        }

        diff = first->mValue;
    }
    else if (auto dFirst = dynamic_cast<Expressions::InexactNumberExpression *>(expr.front().get()))
    {
        if (expr.size() == 1)
        {
            return std::unique_ptr<Expressions::Expression>
                    (new Expressions::InexactNumberExpression(-1 * dFirst->value,
                                                              std::make_unique<Expressions::Scope>(
                                                                      Expressions::Scope(std::move(scope)))));
        }

        returnFloating = true;
        dDiff = dFirst->value;
    }
    else throw std::invalid_argument("- expects numerical arg, found: " + expr.front()->toString());

    for (int i = 1; i < expr.size(); i++)
    {
        auto &j = expr[i];
        if (auto num = dynamic_cast<Expressions::NumericalValueExpression *>(j.get()))
        {
            diff -= num->mValue;
        }
        else if (auto dNum = dynamic_cast<Expressions::InexactNumberExpression *>(j.get()))
        {
            returnFloating = true;
            dDiff -= dNum->value;
        }
        else throw std::invalid_argument("- expects numerical arg, found: " + j->toString());
    }

    if (returnFloating)
    {
        return std::make_unique<Expressions::InexactNumberExpression>
                (Expressions::InexactNumberExpression(dDiff + boost::rational_cast<double>(diff),
                                                      std::move(scope)));
    }

    return std::unique_ptr<Expressions::Expression>
            (new Expressions::NumericalValueExpression(diff,
                                                       std::make_unique<Expressions::Scope>(
                                                               Expressions::Scope(
                                                                       std::move(scope)))));
}

expr_ptr mult_func(expression_vector expr, scope_ptr scope)
{
    boost::rational<int> product(1);
    double dProd = 1.0;
    bool returnFloating = false;

    for (auto &i : expr)
    {
        if (auto num = dynamic_cast<Expressions::NumericalValueExpression *>(i.get()))
        {
            product *= num->mValue;
        }
        else if (auto fl = dynamic_cast<Expressions::InexactNumberExpression *>(i.get()))
        {
            returnFloating = true;
            dProd *= fl->value;
        }
        else throw std::invalid_argument("* expects numerical arg, found: " + i->toString());
    }

    if (returnFloating)
    {
        return std::make_unique<Expressions::InexactNumberExpression>
                (Expressions::InexactNumberExpression(dProd * boost::rational_cast<double>(product),
                                                      std::move(scope)));
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

    boost::rational<int> quotient = 1;
    double dQuot = 1.0;
    bool returnFloating = false;

    if (auto first = dynamic_cast<Expressions::NumericalValueExpression *>(expr.front().get()))
    {
        if (expr.size() == 1)
        {
            return std::unique_ptr<Expressions::Expression>
                    (new Expressions::NumericalValueExpression(1 / first->mValue,
                                                               std::make_unique<Expressions::Scope>(
                                                                       Expressions::Scope(std::move(scope)))));
        }

        quotient = first->mValue;
    }
    else if (auto dFirst = dynamic_cast<Expressions::InexactNumberExpression *>(expr.front().get()))
    {
        if (expr.size() == 1)
        {
            return std::unique_ptr<Expressions::Expression>
                    (new Expressions::InexactNumberExpression(1 / dFirst->value,
                                                              std::make_unique<Expressions::Scope>(
                                                                      Expressions::Scope(std::move(scope)))));
        }

        returnFloating = true;
        dQuot = dFirst->value;
    }
    else throw std::invalid_argument("/ expects numerical arg, found: " + expr.front()->toString());

    for (int i = 1; i < expr.size(); i++)
    {
        auto &j = expr[i];
        if (auto num = dynamic_cast<Expressions::NumericalValueExpression *>(j.get()))
        {
            quotient /= num->mValue;
        }
        else if (auto dNum = dynamic_cast<Expressions::InexactNumberExpression *>(j.get()))
        {
            returnFloating = true;
            dQuot /= dNum->value;
        }
        else throw std::invalid_argument("/ expects numerical arg, found: " + j->toString());
    }

    if (returnFloating)
    {
        return std::make_unique<Expressions::InexactNumberExpression>
                (Expressions::InexactNumberExpression(dQuot * boost::rational_cast<double>(quotient),
                                                      std::move(scope)));
    }

    return std::unique_ptr<Expressions::Expression>(new Expressions::NumericalValueExpression
                                                            (quotient, std::move(scope)));
}

expr_ptr funcSqrt(expression_vector args, scope_ptr scope)
{
    Functions::arg_count_check(args, 1);

    double result = 0;

    if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args.front().get()))
    {
        result = sqrt(boost::rational_cast<double>(rational->mValue));
    }
    else if (auto db = dynamic_cast<Expressions::InexactNumberExpression *>(args.front().get()))
    {
        result = sqrt(db->value);
    }
    else throw std::invalid_argument("sqrt expected number, found " + args.front()->toString());

    return std::make_unique<Expressions::InexactNumberExpression>
            (Expressions::InexactNumberExpression(result, std::move(scope)));
}

expr_ptr funcSqr(expression_vector args, scope_ptr scope)
{
    Functions::arg_count_check(args, 1);

    if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args.front().get()))
    {
        boost::rational<int> result = rational->mValue * rational->mValue;
        return std::unique_ptr<Expressions::Expression>(new Expressions::NumericalValueExpression
                                                                (result, std::move(scope)));
    }
    else if (auto db = dynamic_cast<Expressions::InexactNumberExpression *>(args.front().get()))
    {
        double result = db->value * db->value;
        return std::make_unique<Expressions::InexactNumberExpression>
                (Expressions::InexactNumberExpression(result, std::move(scope)));
    }
    else throw std::invalid_argument("sqrt expected number, found " + args.front()->toString());
}

expr_ptr funcExpt(expression_vector args, scope_ptr scope)
{
    Functions::arg_count_check(args, 2);

    double base, exponent, result;
    bool exactBase = false, exactExp = false;

    if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args[0].get()))
    {
        base = boost::rational_cast<double>(rational->mValue);
        exactBase = true;
    }
    else if (auto db = dynamic_cast<Expressions::InexactNumberExpression *>(args[0].get()))
    {
        base = db->value;
    }
    else throw std::invalid_argument("sqrt expected number, found " + args[0]->toString());

    if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args[1].get()))
    {
        exponent = boost::rational_cast<double>(rational->mValue);
        exactExp = true;
    }
    else if (auto db = dynamic_cast<Expressions::InexactNumberExpression *>(args[1].get()))
    {
        exponent = db->value;
    }
    else throw std::invalid_argument("sqrt expected number, found " + args[1]->toString());

    result = pow(base, exponent);

    if (exactBase && exactExp)
    {
        return std::unique_ptr<Expressions::Expression>(new Expressions::NumericalValueExpression
                                                                (result, std::move(scope)));
    }
    else
    {
        return std::make_unique<Expressions::InexactNumberExpression>
                (Expressions::InexactNumberExpression(result, std::move(scope)));
    }
}

expr_ptr funcMax(expression_vector args, scope_ptr scope)
{
    if (args.empty()) throw std::invalid_argument("Error: Expected at least 1 argument, but found none.");

    boost::rational<int> rationalMax;
    double inexactMax = 0;
    bool firstRational = true, firstInexact = true;

    for (auto &expr : args)
    {
        if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(expr.get()))
        {
            if (firstRational)
            {
                rationalMax = rational->mValue;
                firstRational = false;
            }
            else
            {
                rationalMax = rational->mValue > rationalMax ? rational->mValue : rationalMax;
            }
        }
        else if (auto inexact = dynamic_cast<Expressions::InexactNumberExpression *>(expr.get()))
        {
            if (firstInexact)
            {
                inexactMax = inexact->value;
                firstInexact = false;
            }
            else
            {
                inexactMax = inexact->value > inexactMax ? inexact->value : inexactMax;
            }
        }
        else throw std::invalid_argument("Expected number, found " + expr->toString());
    }

    // If we came across an inexact number, then firstInexact is false
    if (!firstInexact)
    {
        auto rationalM = boost::rational_cast<double>(rationalMax);
        return std::make_unique<Expressions::InexactNumberExpression>
                (Expressions::InexactNumberExpression(
                        (rationalM > inexactMax && !firstRational) ? rationalM : inexactMax,
                        std::move(scope)
                ));
    }

    return std::make_unique<Expressions::NumericalValueExpression>
            (Expressions::NumericalValueExpression(rationalMax, std::move(scope)));
}

expr_ptr funcMin(expression_vector args, scope_ptr scope)
{
    if (args.empty()) throw std::invalid_argument("Error: Expected at least 1 argument, but found none.");

    boost::rational<int> rationalMax;
    double inexactMax = 0;
    bool firstRational = true, firstInexact = true;

    for (auto &expr : args)
    {
        if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(expr.get()))
        {
            if (firstRational)
            {
                rationalMax = rational->mValue;
                firstRational = false;
            }
            else
            {
                rationalMax = rational->mValue < rationalMax ? rational->mValue : rationalMax;
            }
        }
        else if (auto inexact = dynamic_cast<Expressions::InexactNumberExpression *>(expr.get()))
        {
            if (firstInexact)
            {
                inexactMax = inexact->value;
                firstInexact = false;
            }
            else
            {
                inexactMax = inexact->value < inexactMax ? inexact->value : inexactMax;
            }
        }
        else throw std::invalid_argument("Expected number, found " + expr->toString());
    }

    // If we came across an inexact number, then firstInexact is false
    if (!firstInexact)
    {
        auto rationalM = boost::rational_cast<double>(rationalMax);
        return std::make_unique<Expressions::InexactNumberExpression>
                (Expressions::InexactNumberExpression(
                        (rationalM < inexactMax && !firstRational) ? rationalM : inexactMax,
                        std::move(scope)
                ));
    }

    return std::make_unique<Expressions::NumericalValueExpression>
            (Expressions::NumericalValueExpression(rationalMax, std::move(scope)));
}

expr_ptr absFn(expression_vector args, const scope_ptr & /* scope */)
{
    Functions::arg_count_check(args, 1);

    if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args[0].get()))
    {
        if (rational->mValue < 0) rational->mValue = -rational->mValue;

        return std::move(args[0]);
    }
    else if (auto inexact = dynamic_cast<Expressions::InexactNumberExpression *>(args[0].get()))
    {
        if (inexact->value < 0) inexact->value = -inexact->value;

        return std::move(args[0]);
    }
    else throw std::invalid_argument("Expected number, found " + args[0]->toString());
}

expr_ptr floorFn(expression_vector args, const scope_ptr & /* scope */)
{
    Functions::arg_count_check(args, 1);

    if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args[0].get()))
    {
        boost::rational<int> val = rational->mValue;
        int modulo = val.numerator() % val.denominator();

        rational->mValue = val - boost::rational<int>(modulo, val.denominator());

        return std::move(args[0]);
    }
    else if (auto inexact = dynamic_cast<Expressions::InexactNumberExpression *>(args[0].get()))
    {
        inexact->value = floor(inexact->value);

        return std::move(args[0]);
    }
    else throw std::invalid_argument("Expected number, found " + args[0]->toString());
}

expr_ptr ceilFn(expression_vector args, const scope_ptr & /* scope */)
{
    Functions::arg_count_check(args, 1);

    if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args[0].get()))
    {
        boost::rational<int> val = rational->mValue;
        int modulo = val.numerator() % val.denominator();

        if (modulo != 0) modulo -= val.denominator();

        rational->mValue = val - boost::rational<int>(modulo, val.denominator());

        return std::move(args[0]);
    }
    else if (auto inexact = dynamic_cast<Expressions::InexactNumberExpression *>(args[0].get()))
    {
        inexact->value = ceil(inexact->value);

        return std::move(args[0]);
    }
    else throw std::invalid_argument("Expected number, found " + args[0]->toString());
}

expr_ptr zeroPredicate(expression_vector args, scope_ptr scope)
{
    Functions::arg_count_check(args, 1);

    if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args[0].get()))
    {
        return std::make_unique<Expressions::BooleanValueExpression>
                (Expressions::BooleanValueExpression(rational->mValue == 0, std::move(scope)));
    }
    else if (auto inexact = dynamic_cast<Expressions::InexactNumberExpression *>(args[0].get()))
    {
        return std::make_unique<Expressions::BooleanValueExpression>
                (Expressions::BooleanValueExpression(inexact->value == 0, std::move(scope)));
    }
    else throw std::invalid_argument("Expected number, found " + args[0]->toString());
}

void register_math_functions()
{
    Functions::funcMap["+"] = plus_func;
    Functions::funcMap["-"] = sub_func;
    Functions::funcMap["*"] = mult_func;
    Functions::funcMap["/"] = div_func;
    Functions::funcMap["sqrt"] = funcSqrt;
    Functions::funcMap["sqr"] = funcSqr;
    Functions::funcMap["expt"] = funcExpt;
    Functions::funcMap["max"] = funcMax;
    Functions::funcMap["min"] = funcMin;
    Functions::funcMap["abs"] = absFn;
    Functions::funcMap["floor"] = floorFn;
    Functions::funcMap["ceiling"] = ceilFn;
    Functions::funcMap["zero?"] = zeroPredicate;
}

