//
// Created by Antonio Abbatangelo on 2019-06-22.
//

#include "functions.h"

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
}

void register_list_functions()
{
    Functions::funcMap["cons"] = ListFunctions::consFn;
    Functions::funcMap["list"] = ListFunctions::listFn;
    Functions::funcMap["first"] = ListFunctions::firstFn;
    Functions::funcMap["second"] = ListFunctions::secondFn;
    Functions::funcMap["third"] = ListFunctions::thirdFn;
    Functions::funcMap["rest"] = ListFunctions::restFn;
    Functions::funcMap["empty?"] = ListFunctions::emptyPredicateFn;
    Functions::funcMap["cons?"] = ListFunctions::consPredicateFn;
    Functions::funcMap["member?"] = ListFunctions::memberPredicate;
    Functions::funcMap["length"] = ListFunctions::lengthFunction;
    Functions::funcMap["append"] = ListFunctions::appendFunction;
    Functions::funcMap["reverse"] = ListFunctions::reverseFn;
}