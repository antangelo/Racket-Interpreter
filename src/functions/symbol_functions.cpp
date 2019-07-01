//
// Created by Antonio Abbatangelo on 2019-06-15.
//

#include "functions.h"

namespace SymbolFunctions
{
    typedef std::unique_ptr<Expressions::Expression> expr_ptr;
    typedef std::shared_ptr<Expressions::Scope> scope_ptr;
    using Expressions::expression_vector;

    expr_ptr symbolEqualFunc(expression_vector args, scope_ptr scope)
    {
        Functions::arg_count_check(args, 2);
        if (args[0]->type() != "SymbolExpression")
            throw std::invalid_argument("Expected symbol, found " + args[0]->toString());
        if (args[1]->type() != "SymbolExpression")
            throw std::invalid_argument("Expected symbol, found " + args[1]->toString());

        return std::make_unique<Expressions::BooleanValueExpression>
                (Expressions::BooleanValueExpression(
                        args[0]->toString() == args[1]->toString(),
                        std::move(scope)));
    }

    expr_ptr symbolPredicateFunc(expression_vector args, scope_ptr scope)
    {
        Functions::arg_count_check(args, 1);

        return std::make_unique<Expressions::BooleanValueExpression>
                (Expressions::BooleanValueExpression(args[0]->type() == "SymbolExpression", std::move(scope)));
    }

    expr_ptr symbolToStringFunc(expression_vector args, scope_ptr scope)
    {
        Functions::arg_count_check(args, 1);
        if (args[0]->type() != "SymbolExpression")
            throw std::invalid_argument("Expected symbol, found " + args[0]->toString());

        std::string str = args[0]->toString();

        if (str.length() <= 1) throw std::invalid_argument("Invalid symbol: " + str);

        if (str[1] == '|') str = str.substr(2, str.length() - 3);
        else str = str.substr(1);

        return std::make_unique<Expressions::StringExpression>
                (Expressions::StringExpression(str, std::move(scope)));
    }
}

void register_symbol_functions()
{
    Functions::funcMap["symbol=?"] = SymbolFunctions::symbolEqualFunc;
    Functions::funcMap["symbol?"] = SymbolFunctions::symbolPredicateFunc;
    Functions::funcMap["symbol->string"] = SymbolFunctions::symbolToStringFunc;
}