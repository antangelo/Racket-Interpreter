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
        else throw std::invalid_argument("Expected list, found " + args[1]->toString());
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
        else throw std::invalid_argument("Expected list, found " + args[1]->toString());
    }
}

void register_list_functions()
{
    Functions::funcMap["cons"] = ListFunctions::consFn;
    Functions::funcMap["list"] = ListFunctions::listFn;
    Functions::funcMap["first"] = ListFunctions::firstFn;
    Functions::funcMap["rest"] = ListFunctions::restFn;
}