//
// Created by Antonio Abbatangelo on 2019-06-15.
//

#include "functions.h"

typedef std::unique_ptr<Expressions::Expression> expr_ptr;
typedef std::shared_ptr<Expressions::Scope> scope_ptr;
using Expressions::expression_vector;

void validate_string_fn(const expression_vector &args)
{
    for (auto &arg : args)
    {
        if (arg->type() != "StringExpression")
            throw std::invalid_argument("Expected string, found " + arg->toString());
    }
}

expr_ptr stringEqualFunc(expression_vector args, scope_ptr scope)
{
    Functions::arg_count_check(args, 2);
    validate_string_fn(args);

    return std::make_unique<Expressions::BooleanValueExpression>
            (Expressions::BooleanValueExpression(
                    args[0]->toString() == args[1]->toString(),
                    std::move(scope)));
}

expr_ptr stringPredicateFunc(expression_vector args, scope_ptr scope)
{
    Functions::arg_count_check(args, 1);

    return std::make_unique<Expressions::BooleanValueExpression>
            (Expressions::BooleanValueExpression(args[0]->type() == "StringExpression", std::move(scope)));
}

expr_ptr stringToSymbolFunc(expression_vector args, scope_ptr scope)
{
    Functions::arg_count_check(args, 1);

    if (auto arg = dynamic_cast<Expressions::StringExpression *>(args[0].get()))
    {
        std::string str = arg->str;
        bool containsSpace = false;

        for (char &ch : str)
            if (ch == ' ' || ch == '\n' || ch == '\t')
            {
                containsSpace = true;
                break;
            }

        if (containsSpace) str = "'|" + str + "|";
        else if (str.empty()) str = "'||";
        else str = "'" + str;

        return std::make_unique<Expressions::SymbolExpression>
                (Expressions::SymbolExpression(str, std::move(scope)));
    }

    throw std::invalid_argument("Expected string, found " + args[0]->toString());
}

expr_ptr stringToIntFunc(expression_vector args, scope_ptr scope)
{
    Functions::arg_count_check(args, 1);

    if (auto arg = dynamic_cast<Expressions::StringExpression *>(args[0].get()))
    {
        std::string str = arg->str;

        if (str.length() != 1)
            throw std::invalid_argument("Expected string of length 1, found length "
                                        + std::to_string(str.length()));

        char ch = str[0];
        return std::make_unique<Expressions::NumericalValueExpression>
                (Expressions::NumericalValueExpression(ch, std::move(scope)));
    }

    throw std::invalid_argument("Expected string, found " + args[0]->toString());
}

void register_string_functions()
{
    Functions::funcMap["string?"] = stringPredicateFunc;
    Functions::funcMap["string=?"] = stringEqualFunc;
    Functions::funcMap["string->symbol"] = stringToSymbolFunc;
    Functions::funcMap["string->int"] = stringToIntFunc;
}