//
// Created by Antonio Abbatangelo on 2019-06-22.
//

#include "functions.h"
#include "../interpret/interpret.h"
#include "boost/multiprecision/cpp_int.hpp"

namespace Interpreter
{
    void print(Expressions::expression_vector &steps);
}

std::unique_ptr<Expressions::Expression>
funcMin(Expressions::expression_vector args, std::shared_ptr<Expressions::Scope> scope);

namespace ListFunctions
{
    typedef std::unique_ptr<Expressions::Expression> expr_ptr;
    typedef std::shared_ptr<Expressions::Scope> scope_ptr;
    using Expressions::expression_vector;

    expr_ptr consFn(expression_vector args, const scope_ptr & /* scope */)
    {
        Functions::arg_count_check(args, 2);

        if (auto list = dynamic_cast<Expressions::ListExpression *>(args[1].get()))
        {
            list->list.push_front(std::move(args[0]));
            return std::move(args[1]);
        }
        else throw std::invalid_argument("Expected list, found " + args[1]->toString());
    }

    expr_ptr listFn(expression_vector args, scope_ptr scope)
    {
        std::list<std::unique_ptr<Expressions::Expression>> list;

        for (auto &expr : args)
        {
            list.push_back(std::move(expr));
        }

        return std::make_unique<Expressions::ListExpression>
                (Expressions::ListExpression(std::move(list), std::move(scope)));
    }

    expr_ptr firstFn(expression_vector args, const scope_ptr & /* scope */)
    {
        Functions::arg_count_check(args, 1);

        if (auto list = dynamic_cast<Expressions::ListExpression *>(args[0].get()))
        {
            if (list->list.empty()) throw std::invalid_argument("Expected non-empty list, given " + list->toString());

            return std::move(list->list.front());
        }
        else throw std::invalid_argument("Expected list, found " + args[0]->toString());
    }

    expr_ptr secondFn(expression_vector args, const scope_ptr & /* scope */)
    {
        Functions::arg_count_check(args, 1);

        if (auto list = dynamic_cast<Expressions::ListExpression *>(args[0].get()))
        {
            if (list->list.empty()) throw std::invalid_argument("Expected non-empty list, given " + list->toString());

            list->list.pop_front();
            if (list->list.empty()) throw std::invalid_argument("Expected non-empty list, given " + list->toString());

            return std::move(list->list.front());
        }
        else throw std::invalid_argument("Expected list, found " + args[0]->toString());
    }

    expr_ptr thirdFn(expression_vector args, const scope_ptr & /* scope */)
    {
        Functions::arg_count_check(args, 1);

        if (auto list = dynamic_cast<Expressions::ListExpression *>(args[0].get()))
        {
            if (list->list.empty()) throw std::invalid_argument("Expected non-empty list, given " + list->toString());

            list->list.pop_front();
            if (list->list.empty()) throw std::invalid_argument("Expected non-empty list, given " + list->toString());

            list->list.pop_front();
            if (list->list.empty()) throw std::invalid_argument("Expected non-empty list, given " + list->toString());

            return std::move(list->list.front());
        }
        else throw std::invalid_argument("Expected list, found " + args[0]->toString());
    }

    expr_ptr fourthFn(expression_vector args, const scope_ptr & /* scope */)
    {
        Functions::arg_count_check(args, 1);

        if (auto list = dynamic_cast<Expressions::ListExpression *>(args[0].get()))
        {
            if (list->list.empty()) throw std::invalid_argument("Expected non-empty list, given " + list->toString());

            list->list.pop_front();
            if (list->list.empty()) throw std::invalid_argument("Expected non-empty list, given " + list->toString());

            list->list.pop_front();
            if (list->list.empty()) throw std::invalid_argument("Expected non-empty list, given " + list->toString());

            list->list.pop_front();
            if (list->list.empty()) throw std::invalid_argument("Expected non-empty list, given " + list->toString());

            return std::move(list->list.front());
        }
        else throw std::invalid_argument("Expected list, found " + args[0]->toString());
    }

    expr_ptr restFn(expression_vector args, const scope_ptr & /* scope */)
    {
        Functions::arg_count_check(args, 1);

        if (auto list = dynamic_cast<Expressions::ListExpression *>(args[0].get()))
        {
            if (list->list.empty()) throw std::invalid_argument("Expected non-empty list, given " + list->toString());

            list->list.pop_front();
            return std::move(args[0]);
        }
        else throw std::invalid_argument("Expected list, found " + args[0]->toString());
    }

    expr_ptr emptyPredicateFn(expression_vector args, scope_ptr scope)
    {
        Functions::arg_count_check(args, 1);
        bool isEmpty = false;

        if (auto list = dynamic_cast<Expressions::ListExpression *>(args[0].get()))
        {
            isEmpty = list->list.empty();
        }

        return std::make_unique<Expressions::BooleanValueExpression>
                (Expressions::BooleanValueExpression(isEmpty, std::move(scope)));
    }

    expr_ptr consPredicateFn(expression_vector args, scope_ptr scope)
    {
        Functions::arg_count_check(args, 1);
        bool isCons = false;

        if (auto list = dynamic_cast<Expressions::ListExpression *>(args[0].get()))
        {
            isCons = !list->list.empty();
        }

        return std::make_unique<Expressions::BooleanValueExpression>
                (Expressions::BooleanValueExpression(isCons, std::move(scope)));
    }

    expr_ptr memberPredicate(expression_vector args, scope_ptr scope)
    {
        Functions::arg_count_check(args, 2);

        if (auto list = dynamic_cast<Expressions::ListExpression *>(args[1].get()))
        {
            std::string test = args[0]->toString();
            bool member = false;
            for (auto &expr : list->list)
            {
                member = expr->toString() == test;
                if (member) break;
            }

            return std::make_unique<Expressions::BooleanValueExpression>
                    (Expressions::BooleanValueExpression(member, std::move(scope)));
        }

        throw std::invalid_argument("Expected list, found " + args[1]->toString());
    }

    expr_ptr lengthFunction(expression_vector args, scope_ptr scope)
    {
        Functions::arg_count_check(args, 1);

        if (auto list = dynamic_cast<Expressions::ListExpression *>(args[0].get()))
        {
            return std::make_unique<Expressions::NumericalValueExpression>
                    (Expressions::NumericalValueExpression(list->list.size(), std::move(scope)));
        }

        throw std::invalid_argument("Expected list, found " + args[0]->toString());
    }

    expr_ptr appendFunction(expression_vector args, const scope_ptr &/* scope */)
    {
        if (args.empty()) throw std::invalid_argument("Expected at least one argument, found none");

        if (auto l1 = dynamic_cast<Expressions::ListExpression *>(args[0].get()))
        {
            for (int i = 1; i < args.size(); ++i)
            {
                if (auto list = dynamic_cast<Expressions::ListExpression *>(args[i].get()))
                {
                    for (auto &elem : list->list)
                    {
                        l1->list.push_back(std::move(elem));
                    }
                }
                else throw std::invalid_argument("Expected list, found " + args[i]->toString());
            }

            return std::move(args[0]);
        }

        throw std::invalid_argument("Expected list, found " + args[0]->toString());
    }

    expr_ptr reverseFn(expression_vector args, const scope_ptr & /* scope */)
    {
        Functions::arg_count_check(args, 1);

        if (auto list = dynamic_cast<Expressions::ListExpression *>(args[0].get()))
        {
            list->list.reverse();

            return std::move(args[0]);
        }
        else throw std::invalid_argument("Expected list, found " + args[0]->toString());
    }

    int assertListsAreOfEqualSize(const expression_vector &listVector, int startIndex)
    {
        int length = -1;

        for (int i = startIndex; i < listVector.size(); ++i)
        {
            if (auto list = dynamic_cast<Expressions::ListExpression *>(listVector[i].get()))
            {
                if (length == -1) length = list->list.size();
                else if (length != list->list.size())
                    throw std::invalid_argument(
                            "List length mismatch, expected " + std::to_string(length) + ", found " +
                            std::to_string(list->list.size()));
            }
            else throw std::invalid_argument("Expected list, found " + listVector[i]->toString());
        }

        if (length == -1) throw std::invalid_argument("Expected at least one list, found none");
        return length;
    }

    expression_vector getFuncCallParamsBack(expression_vector &args, int listStartIndex)
    {
        expression_vector params;
        for (int listIndex = listStartIndex; listIndex < args.size(); ++listIndex)
        {
            if (auto list = dynamic_cast<Expressions::ListExpression *>(args[listIndex].get()))
            {
                if (list->list.empty()) throw std::invalid_argument("Found empty list, expected non-empty");

                params.push_back(std::move(list->list.back()));
                list->list.pop_back();
            }
            else throw std::invalid_argument("Expected list, found " + args[listIndex]->toString());
        }

        return std::move(params);
    }

    expression_vector getFuncCallParamsFront(expression_vector &args, int listStartIndex)
    {
        expression_vector params;
        for (int listIndex = listStartIndex; listIndex < args.size(); ++listIndex)
        {
            if (auto list = dynamic_cast<Expressions::ListExpression *>(args[listIndex].get()))
            {
                if (list->list.empty()) throw std::invalid_argument("Found empty list, expected non-empty");

                params.push_back(std::move(list->list.front()));
                list->list.pop_front();
            }
            else throw std::invalid_argument("Expected list, found " + args[listIndex]->toString());
        }

        return std::move(params);
    }

    expr_ptr foldr(expression_vector args, const scope_ptr & /* scope */)
    {
        if (args.size() < 3)
            throw std::invalid_argument("Expected at least 3 arguments, found " + std::to_string(args.size()));

        if (auto f = dynamic_cast<Expressions::FunctionExpression *>(args[0].get()))
        {
            std::unique_ptr<Expressions::Expression> result = std::move(args[1]);
            int elems = assertListsAreOfEqualSize(args, 2);

            for (int i = 0; i < elems; ++i)
            {
                expression_vector params = getFuncCallParamsBack(args, 2);
                params.push_back(std::move(result));
                result = f->call(std::move(params));
            }

            return std::move(result);
        }
        else throw std::invalid_argument("Expected function, found " + args[0]->toString());
    }

    expr_ptr foldl(expression_vector args, const scope_ptr & /* scope */)
    {
        if (args.size() < 3)
            throw std::invalid_argument("Expected at least 3 arguments, found " + std::to_string(args.size()));

        if (auto f = dynamic_cast<Expressions::FunctionExpression *>(args[0].get()))
        {
            std::unique_ptr<Expressions::Expression> result = std::move(args[1]);
            int elems = assertListsAreOfEqualSize(args, 2);

            for (int i = 0; i < elems; ++i)
            {
                expression_vector params = getFuncCallParamsFront(args, 2);
                params.push_back(std::move(result));
                result = f->call(std::move(params));
            }

            return std::move(result);
        }
        else throw std::invalid_argument("Expected function, found " + args[0]->toString());
    }

    expr_ptr filter(expression_vector args, const scope_ptr & /* scope */)
    {
        Functions::arg_count_check(args, 2);

        if (auto f = dynamic_cast<Expressions::FunctionExpression *>(args[0].get()))
        {
            if (auto list = dynamic_cast<Expressions::ListExpression *> (args[1].get()))
            {
                std::list<std::unique_ptr<Expressions::Expression>> listFinal;
                for (auto &elem : list->list)
                {
                    expression_vector params;
                    params.push_back(elem->clone());
                    expr_ptr test = Interpreter::interpret(f->call(std::move(params)));

                    if (auto b = dynamic_cast<Expressions::BooleanValueExpression *>(test.get()))
                    {
                        if (b->value) listFinal.push_back(std::move(elem));
                    }
                    else throw std::invalid_argument("Expected boolean, found " + test->toString());
                }

                list->list.clear();
                list->list = std::move(listFinal);
                return std::move(args[1]);
            }
            else throw std::invalid_argument("Expected list, found " + args[1]->toString());
        }
        else throw std::invalid_argument("Expected function, found " + args[0]->toString());
    }

    std::function<bool(const expr_ptr &, const expr_ptr &)>
    racketPredicateAdapter(Expressions::FunctionExpression &function)
    {
        return [&function](const expr_ptr &arg0, const expr_ptr &arg1) -> bool
        {
            expression_vector args = expression_vector();
            args.push_back(arg0->clone());
            args.push_back(arg1->clone());

            expr_ptr expr = function.call(std::move(args));
            if (!expr->isValue()) expr = Interpreter::interpret(std::move(expr));
            if (auto boolVal = dynamic_cast<Expressions::BooleanValueExpression *>(expr.get()))
            {
                return boolVal->value;
            }
            else throw std::invalid_argument("Function " + function.toString() + " cannot be cast to bool");
        };
    }

    expr_ptr sortFn(expression_vector args, const scope_ptr &/* scope */)
    {
        Functions::arg_count_check(args, 2);

        if (auto list = dynamic_cast<Expressions::ListExpression *>(args[0].get()))
        {
            if (auto func = dynamic_cast<Expressions::FunctionExpression *>(args[1].get()))
            {
                list->list.sort(racketPredicateAdapter(*func));
                return std::move(args[0]);
            }
            else throw std::invalid_argument("Expected function, found " + args[1]->toString());
        }
        else throw std::invalid_argument("Expected list, found " + args[0]->toString());
    }

    expr_ptr mapFn(expression_vector args, scope_ptr scope)
    {
        if (args.size() < 2)
            throw std::invalid_argument("Expected at least 2 arguments, found " + std::to_string(args.size()));

        if (auto f = dynamic_cast<Expressions::FunctionExpression *>(args[0].get()))
        {
            std::list<expr_ptr> returnList;
            int elems = assertListsAreOfEqualSize(args, 1);

            for (int i = 0; i < elems; ++i)
            {
                expression_vector params = getFuncCallParamsFront(args, 1);
                returnList.push_back(Interpreter::interpret(f->call(std::move(params))));
            }

            return std::make_unique<Expressions::ListExpression>(
                    Expressions::ListExpression(std::move(returnList), std::move(scope)));
        }
        else throw std::invalid_argument("Expected function, found " + args[0]->toString());
    }

    expr_ptr buildList(expression_vector args, scope_ptr scope)
    {
        Functions::arg_count_check(args, 2);

        if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(args[0].get()))
        {
            if (rational->value.denominator() == 1)
            {
                if (auto func = dynamic_cast<Expressions::FunctionExpression *>(args[1].get()))
                {
                    std::list<expr_ptr> list;

                    for (boost::multiprecision::cpp_int i = 0; i < rational->value.numerator(); ++i)
                    {
                        expression_vector params;
                        params.push_back(std::make_unique<Expressions::NumericalValueExpression>(
                                Expressions::NumericalValueExpression(i, scope)));
                        list.push_back(Interpreter::interpret(func->call(std::move(params))));
                    }

                    return std::make_unique<Expressions::ListExpression>(
                            Expressions::ListExpression(std::move(list), std::move(scope)));
                }
                else throw std::invalid_argument("Expected function, found " + args[1]->toString());
            }
        }

        throw std::invalid_argument("Expected natural number, found " + args[0]->toString());
    }

    bool n2Smaller(const expr_ptr &number1, const expr_ptr &number2)
    {
        Expressions::InexactNumberExpression::numerical_type n1Val, n2Val;

        if (auto n1Rational = dynamic_cast<Expressions::NumericalValueExpression *>(number1.get()))
        {
            n1Val = boost::rational_cast<Expressions::InexactNumberExpression::numerical_type>(n1Rational->value);
        }
        else if (auto n1Inexact = dynamic_cast<Expressions::InexactNumberExpression *>(number1.get()))
        {
            n1Val = n1Inexact->value;
        }
        else throw std::invalid_argument("Expected number, found " + number1->toString());

        if (auto n2Rational = dynamic_cast<Expressions::NumericalValueExpression *>(number2.get()))
        {
            n2Val = boost::rational_cast<Expressions::InexactNumberExpression::numerical_type>(n2Rational->value);
        }
        else if (auto n2Inexact = dynamic_cast<Expressions::InexactNumberExpression *>(number2.get()))
        {
            n2Val = n2Inexact->value;
        }
        else throw std::invalid_argument("Expected number, found " + number2->toString());

        return n1Val > n2Val;
    }

    expr_ptr argminFn(expression_vector args, const scope_ptr & /* scope */)
    {
        Functions::arg_count_check(args, 2);

        if (auto func = dynamic_cast<Expressions::FunctionExpression *>(args[0].get()))
        {
            if (auto list = dynamic_cast<Expressions::ListExpression *>(args[1].get()))
            {
                std::unique_ptr<Expressions::Expression> min = nullptr;
                std::unique_ptr<Expressions::Expression> minNumber = nullptr;

                for (auto &expr : list->list)
                {
                    if (!min)
                    {
                        Expressions::expression_vector fnArgs;
                        fnArgs.push_back(expr->clone());
                        minNumber = Interpreter::interpret(func->call(std::move(fnArgs)));
                        min = std::move(expr);
                    }
                    else
                    {
                        Expressions::expression_vector fnArgs;
                        fnArgs.push_back(expr->clone());
                        auto newNumber = Interpreter::interpret(func->call(std::move(fnArgs)));

                        if (n2Smaller(minNumber, newNumber))
                        {
                            min = std::move(expr);
                            minNumber = std::move(newNumber);
                        }
                    }
                }

                return std::move(min);
            }
            else throw std::invalid_argument("Expected list, found " + args[1]->toString());
        }
        else throw std::invalid_argument("Expected function, found " + args[0]->toString());
    }
}

void register_list_functions()
{
    Functions::funcMap["cons"] = ListFunctions::consFn;
    Functions::funcMap["list"] = ListFunctions::listFn;
    Functions::funcMap["first"] = ListFunctions::firstFn;
    Functions::funcMap["second"] = ListFunctions::secondFn;
    Functions::funcMap["third"] = ListFunctions::thirdFn;
    Functions::funcMap["fourth"] = ListFunctions::fourthFn;
    Functions::funcMap["rest"] = ListFunctions::restFn;
    Functions::funcMap["empty?"] = ListFunctions::emptyPredicateFn;
    Functions::funcMap["cons?"] = ListFunctions::consPredicateFn;
    Functions::funcMap["member?"] = ListFunctions::memberPredicate;
    Functions::funcMap["length"] = ListFunctions::lengthFunction;
    Functions::funcMap["append"] = ListFunctions::appendFunction;
    Functions::funcMap["reverse"] = ListFunctions::reverseFn;

    Functions::funcMap["foldr"] = ListFunctions::foldr;
    Functions::funcMap["foldl"] = ListFunctions::foldl;
    Functions::funcMap["filter"] = ListFunctions::filter;
    Functions::funcMap["sort"] = ListFunctions::sortFn;
    Functions::funcMap["map"] = ListFunctions::mapFn;
    Functions::funcMap["build-list"] = ListFunctions::buildList;
    Functions::funcMap["argmin"] = ListFunctions::argminFn;
}