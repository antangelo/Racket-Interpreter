//
// Created by Antonio Abbatangelo on 2019-05-26.
//

#include "functions.h"
#include "../interpret/parser.h"
#include <cmath>

#include "boost/multiprecision/gmp.hpp"

namespace MathFunctions
{

    typedef std::unique_ptr<Expressions::Expression> expr_ptr;
    typedef std::shared_ptr<Expressions::Scope> scope_ptr;
    using Expressions::expression_vector;

    expr_ptr plus_func(expression_vector expr, scope_ptr scope)
    {
        Expressions::NumericalValueExpression::numerical_type sum = 0;
        Expressions::InexactNumberExpression::numerical_type dSum = 0.0;
        bool returnFloating = false;

        for (auto &i : expr)
        {
            if (auto num = dynamic_cast<Expressions::NumericalValueExpression *>(i.get()))
            {
                sum += num->value;
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
            auto numerator = boost::multiprecision::numerator(sum);
            auto denominator = boost::multiprecision::denominator(sum);
            auto inexactSum = Expressions::InexactNumberExpression::numerical_type(numerator);
            inexactSum /= Expressions::InexactNumberExpression::numerical_type(denominator);
            return std::make_unique<Expressions::InexactNumberExpression>
                    (Expressions::InexactNumberExpression(inexactSum + dSum,
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

        Expressions::NumericalValueExpression::numerical_type diff = 0;
        Expressions::InexactNumberExpression::numerical_type dDiff = 0.0;
        bool returnFloating = false;

        if (auto first = dynamic_cast<Expressions::NumericalValueExpression *>(expr.front().get()))
        {
            if (expr.size() == 1)
            {
                return std::unique_ptr<Expressions::Expression>
                        (new Expressions::NumericalValueExpression(-1 * first->value,
                                                                   std::make_unique<Expressions::Scope>(
                                                                           Expressions::Scope(std::move(scope)))));
            }

            diff = first->value;
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
                diff -= num->value;
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
            auto numerator = boost::multiprecision::numerator(diff);
            auto denominator = boost::multiprecision::denominator(diff);
            auto inexactSum = Expressions::InexactNumberExpression::numerical_type(numerator);
            inexactSum /= Expressions::InexactNumberExpression::numerical_type(denominator);
            return std::make_unique<Expressions::InexactNumberExpression>
                    (Expressions::InexactNumberExpression(inexactSum + dDiff,
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
        Expressions::NumericalValueExpression::numerical_type product(1);
        Expressions::InexactNumberExpression::numerical_type dProd = 1.0;
        bool returnFloating = false;

        for (auto &i : expr)
        {
            if (auto num = dynamic_cast<Expressions::NumericalValueExpression *>(i.get()))
            {
                product *= num->value;
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
            auto numerator = boost::multiprecision::numerator(product);
            auto denominator = boost::multiprecision::denominator(product);
            auto inexactSum = Expressions::InexactNumberExpression::numerical_type(numerator);
            inexactSum /= Expressions::InexactNumberExpression::numerical_type(denominator);
            return std::make_unique<Expressions::InexactNumberExpression>
                    (Expressions::InexactNumberExpression(inexactSum * dProd,
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

        Expressions::NumericalValueExpression::numerical_type quotient = 1;
        Expressions::InexactNumberExpression::numerical_type dQuot = 1.0;
        bool returnFloating = false;

        if (auto first = dynamic_cast<Expressions::NumericalValueExpression *>(expr.front().get()))
        {
            if (expr.size() == 1)
            {
                return std::unique_ptr<Expressions::Expression>
                        (new Expressions::NumericalValueExpression(quotient / first->value,
                                                                   std::make_unique<Expressions::Scope>(
                                                                           Expressions::Scope(std::move(scope)))));
            }

            quotient = first->value;
        }
        else if (auto dFirst = dynamic_cast<Expressions::InexactNumberExpression *>(expr.front().get()))
        {
            if (expr.size() == 1)
            {
                return std::unique_ptr<Expressions::Expression>
                        (new Expressions::InexactNumberExpression(dQuot / dFirst->value,
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
                quotient /= num->value;
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
            auto numerator = boost::multiprecision::numerator(quotient);
            auto denominator = boost::multiprecision::denominator(quotient);
            auto inexactQuot = Expressions::InexactNumberExpression::numerical_type(numerator);
            inexactQuot /= Expressions::InexactNumberExpression::numerical_type(denominator);
            return std::make_unique<Expressions::InexactNumberExpression>
                    (Expressions::InexactNumberExpression(inexactQuot * dQuot,
                                                          std::move(scope)));
        }

        return std::unique_ptr<Expressions::Expression>(new Expressions::NumericalValueExpression
                                                                (quotient, std::move(scope)));
    }

    expr_ptr funcSqrt(expression_vector args, scope_ptr scope)
    {
        Functions::arg_count_check(args, 1);

        Expressions::InexactNumberExpression::numerical_type result = 0;

        if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args.front().get()))
        {
            result = sqrt(Expressions::InexactNumberExpression::numerical_type(rational->value));
        }
        else if (auto db = dynamic_cast<Expressions::InexactNumberExpression *>(args.front().get()))
        {
            result = boost::multiprecision::sqrt(db->value);
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
            Expressions::NumericalValueExpression::numerical_type result = rational->value * rational->value;
            return std::unique_ptr<Expressions::Expression>(new Expressions::NumericalValueExpression
                                                                    (result, std::move(scope)));
        }
        else if (auto db = dynamic_cast<Expressions::InexactNumberExpression *>(args.front().get()))
        {
            Expressions::InexactNumberExpression::numerical_type result = db->value * db->value;
            return std::make_unique<Expressions::InexactNumberExpression>
                    (Expressions::InexactNumberExpression(result, std::move(scope)));
        }
        else throw std::invalid_argument("sqr expected number, found " + args.front()->toString());
    }

    expr_ptr funcExpt(expression_vector args, scope_ptr scope)
    {
        Functions::arg_count_check(args, 2);

        Expressions::InexactNumberExpression::numerical_type base, exponent, result;
        bool exact = false;

        if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args[0].get()))
        {
            base = Expressions::InexactNumberExpression::numerical_type(rational->value);
            exact = true;
        }
        else if (auto db = dynamic_cast<Expressions::InexactNumberExpression *>(args[0].get()))
        {
            base = db->value;
        }
        else throw std::invalid_argument("sqrt expected number, found " + args[0]->toString());

        if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args[1].get()))
        {
            exponent = Expressions::InexactNumberExpression::numerical_type(rational->value);
        }
        else if (auto db = dynamic_cast<Expressions::InexactNumberExpression *>(args[1].get()))
        {
            exponent = db->value;
            exact = false;
        }
        else throw std::invalid_argument("sqrt expected number, found " + args[1]->toString());

        result = boost::multiprecision::pow(base, exponent);

        if (exact)
        {
            auto x = boost::multiprecision::mpz_int(result);
            return std::make_unique<Expressions::NumericalValueExpression>
                (x, boost::multiprecision::mpz_int("1"), std::move(scope));
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

        Expressions::NumericalValueExpression::numerical_type rationalMax;
        Expressions::InexactNumberExpression::numerical_type inexactMax = 0;
        bool firstRational = true, firstInexact = true;

        for (auto &expr : args)
        {
            if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(expr.get()))
            {
                if (firstRational)
                {
                    rationalMax = rational->value;
                    firstRational = false;
                }
                else
                {
                    rationalMax = rational->value > rationalMax ? rational->value : rationalMax;
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
            auto rationalM = Expressions::InexactNumberExpression::numerical_type(rationalMax);
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

        Expressions::NumericalValueExpression::numerical_type rationalMax;
        Expressions::InexactNumberExpression::numerical_type inexactMax = 0;
        bool firstRational = true, firstInexact = true;

        for (auto &expr : args)
        {
            if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(expr.get()))
            {
                if (firstRational)
                {
                    rationalMax = rational->value;
                    firstRational = false;
                }
                else
                {
                    rationalMax = rational->value < rationalMax ? rational->value : rationalMax;
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
            auto rationalM = Expressions::InexactNumberExpression::numerical_type(rationalMax);
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
            if (rational->value < 0) rational->value = -rational->value;

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
            Expressions::NumericalValueExpression::numerical_type val = rational->value;
            auto denominator = boost::multiprecision::denominator(val);
            boost::multiprecision::mpz_int modulo = boost::multiprecision::numerator(val);
            modulo %= denominator;

            rational->value = val - Expressions::NumericalValueExpression::numerical_type(modulo, denominator);

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
            Expressions::NumericalValueExpression::numerical_type val = rational->value;
            auto denominator = boost::multiprecision::denominator(val);
            boost::multiprecision::mpz_int modulo = boost::multiprecision::numerator(val) % denominator;

            if (modulo != 0) modulo -= denominator;

            rational->value = val - Expressions::NumericalValueExpression::numerical_type(modulo, denominator);

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
                    (Expressions::BooleanValueExpression(rational->value == 0, std::move(scope)));
        }
        else if (auto inexact = dynamic_cast<Expressions::InexactNumberExpression *>(args[0].get()))
        {
            return std::make_unique<Expressions::BooleanValueExpression>
                    (Expressions::BooleanValueExpression(inexact->value == 0, std::move(scope)));
        }
        else throw std::invalid_argument("Expected number, found " + args[0]->toString());
    }

    expr_ptr negativePredicate(expression_vector args, scope_ptr scope)
    {
        Functions::arg_count_check(args, 1);

        if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args[0].get()))
        {
            return std::make_unique<Expressions::BooleanValueExpression>
                    (Expressions::BooleanValueExpression(rational->value < 0, std::move(scope)));
        }
        else if (auto inexact = dynamic_cast<Expressions::InexactNumberExpression *>(args[0].get()))
        {
            return std::make_unique<Expressions::BooleanValueExpression>
                    (Expressions::BooleanValueExpression(inexact->value < 0, std::move(scope)));
        }
        else throw std::invalid_argument("Expected number, found " + args[0]->toString());
    }

    expr_ptr positivePredicate(expression_vector args, scope_ptr scope)
    {
        Functions::arg_count_check(args, 1);

        if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args[0].get()))
        {
            return std::make_unique<Expressions::BooleanValueExpression>
                    (Expressions::BooleanValueExpression(rational->value > 0, std::move(scope)));
        }
        else if (auto inexact = dynamic_cast<Expressions::InexactNumberExpression *>(args[0].get()))
        {
            return std::make_unique<Expressions::BooleanValueExpression>
                    (Expressions::BooleanValueExpression(inexact->value > 0, std::move(scope)));
        }
        else throw std::invalid_argument("Expected number, found " + args[0]->toString());
    }

    expr_ptr oddPredicate(expression_vector args, scope_ptr scope)
    {
        Functions::arg_count_check(args, 1);

        if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args[0].get()))
        {
            bool result = boost::multiprecision::denominator(rational->value) == 1 ? 
                boost::multiprecision::numerator(rational->value) % 2 != 0 : false;

            return std::make_unique<Expressions::BooleanValueExpression>
                    (Expressions::BooleanValueExpression(result, std::move(scope)));
        }
        else if (auto inexact = dynamic_cast<Expressions::InexactNumberExpression *>(args[0].get()))
        {
            bool result = inexact->value - (int) inexact->value == 0 ?
                          (int) inexact->value % 2 != 0 : false;

            return std::make_unique<Expressions::BooleanValueExpression>
                    (Expressions::BooleanValueExpression(result, std::move(scope)));
        }
        else throw std::invalid_argument("Expected number, found " + args[0]->toString());
    }

    expr_ptr evenPredicate(expression_vector args, scope_ptr scope)
    {
        Functions::arg_count_check(args, 1);

        if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args[0].get()))
        {
            bool result = boost::multiprecision::denominator(rational->value) == 1 ?
                boost::multiprecision::numerator(rational->value) % 2 == 0 : false;

            return std::make_unique<Expressions::BooleanValueExpression>
                    (Expressions::BooleanValueExpression(result, std::move(scope)));
        }
        else if (auto inexact = dynamic_cast<Expressions::InexactNumberExpression *>(args[0].get()))
        {
            bool result = inexact->value - (int) inexact->value == 0 ?
                          (int) inexact->value % 2 == 0 : false;

            return std::make_unique<Expressions::BooleanValueExpression>
                    (Expressions::BooleanValueExpression(result, std::move(scope)));
        }
        else throw std::invalid_argument("Expected number, found " + args[0]->toString());
    }

    expr_ptr addOneFn(expression_vector args, const scope_ptr & /* scope */)
    {
        Functions::arg_count_check(args, 1);

        if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args[0].get()))
        {
            rational->value += 1;

            return std::move(args[0]);
        }
        else if (auto inexact = dynamic_cast<Expressions::InexactNumberExpression *>(args[0].get()))
        {
            inexact->value += 1;

            return std::move(args[0]);
        }
        else throw std::invalid_argument("Expected number, found " + args[0]->toString());
    }

    expr_ptr subOneFn(expression_vector args, const scope_ptr & /* scope */)
    {
        Functions::arg_count_check(args, 1);

        if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args[0].get()))
        {
            rational->value -= 1;

            return std::move(args[0]);
        }
        else if (auto inexact = dynamic_cast<Expressions::InexactNumberExpression *>(args[0].get()))
        {
            inexact->value -= 1;

            return std::move(args[0]);
        }
        else throw std::invalid_argument("Expected number, found " + args[0]->toString());
    }

    expr_ptr integerPredicate(expression_vector args, scope_ptr scope)
    {
        Functions::arg_count_check(args, 1);

        if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args[0].get()))
        {
            return std::make_unique<Expressions::BooleanValueExpression>
                    (Expressions::BooleanValueExpression(boost::multiprecision::denominator(rational->value) == 1,
                        std::move(scope)));
        }
        else if (auto inexact = dynamic_cast<Expressions::InexactNumberExpression *>(args[0].get()))
        {
            return std::make_unique<Expressions::BooleanValueExpression>
                    (Expressions::BooleanValueExpression(inexact->value - (int) inexact->value == 0, std::move(scope)));
        }
        else
            return std::make_unique<Expressions::BooleanValueExpression>
                    (Expressions::BooleanValueExpression(false, std::move(scope)));
    }

    expr_ptr sineFn(expression_vector args, scope_ptr scope)
    {
        Functions::arg_count_check(args, 1);

        if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args[0].get()))
        {
            return std::make_unique<Expressions::InexactNumberExpression>(
                    Expressions::InexactNumberExpression(boost::multiprecision::sin(
                            Expressions::InexactNumberExpression::numerical_type(
                                    rational->value)),
                                                         std::move(scope)));
        }
        else if (auto inexact = dynamic_cast<Expressions::InexactNumberExpression *>(args[0].get()))
        {
            inexact->value = boost::multiprecision::sin(inexact->value);

            return std::move(args[0]);
        }
        else throw std::invalid_argument("Expected number, found " + args[0]->toString());
    }

    expr_ptr inexactToExactFn(expression_vector args, scope_ptr scope) {
        Functions::arg_count_check(args, 1);

        if(auto inexact = dynamic_cast<Expressions::InexactNumberExpression *>(args[0].get())) {
            return std::make_unique<Expressions::NumericalValueExpression>(
                Expressions::NumericalValueExpression::numerical_type(inexact->value),
                std::move(scope)
            );
        }
        else throw std::invalid_argument("Expected inexact, found " + args[0]->toString());
    }
}

void register_math_functions()
{
    Functions::funcMap["+"] = MathFunctions::plus_func;
    Functions::funcMap["-"] = MathFunctions::sub_func;
    Functions::funcMap["*"] = MathFunctions::mult_func;
    Functions::funcMap["/"] = MathFunctions::div_func;
    Functions::funcMap["sqrt"] = MathFunctions::funcSqrt;
    Functions::funcMap["sqr"] = MathFunctions::funcSqr;
    Functions::funcMap["expt"] = MathFunctions::funcExpt;
    Functions::funcMap["max"] = MathFunctions::funcMax;
    Functions::funcMap["min"] = MathFunctions::funcMin;
    Functions::funcMap["abs"] = MathFunctions::absFn;
    Functions::funcMap["floor"] = MathFunctions::floorFn;
    Functions::funcMap["ceiling"] = MathFunctions::ceilFn;
    Functions::funcMap["zero?"] = MathFunctions::zeroPredicate;
    Functions::funcMap["negative?"] = MathFunctions::negativePredicate;
    Functions::funcMap["positive?"] = MathFunctions::positivePredicate;
    Functions::funcMap["odd?"] = MathFunctions::oddPredicate;
    Functions::funcMap["even?"] = MathFunctions::evenPredicate;
    Functions::funcMap["add1"] = MathFunctions::addOneFn;
    Functions::funcMap["sub1"] = MathFunctions::subOneFn;
    Functions::funcMap["integer?"] = MathFunctions::integerPredicate;
    Functions::funcMap["sin"] = MathFunctions::sineFn;
    Functions::funcMap["itoe"] = MathFunctions::inexactToExactFn;
}

