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

expr_ptr intToStringFunc(expression_vector args, scope_ptr scope)
{
    Functions::arg_count_check(args, 1);

    if (auto arg = dynamic_cast<Expressions::NumericalValueExpression *>(args[0].get()))
    {
        boost::rational<int> num = arg->mValue;
        if (num.denominator() != 1)
            throw std::invalid_argument("Expected rational, found " + args[0]->toString());

        std::string str;
        str += (char) num.numerator();

        return std::make_unique<Expressions::StringExpression>
                (Expressions::StringExpression(str, std::move(scope)));
    }

    throw std::invalid_argument("Expected rational, found " + args[0]->toString());
}

expr_ptr stringToListFn(expression_vector args, scope_ptr scope)
{
    Functions::arg_count_check(args, 1);

    if (auto str = dynamic_cast<Expressions::StringExpression *>(args[0].get()))
    {
        std::list<std::unique_ptr<Expressions::Expression>> list;

        for (auto &chr : str->str)
        {
            list.push_back(std::make_unique<Expressions::CharacterExpression>(Expressions::CharacterExpression(chr,
                                                                                                               std::make_shared<Expressions::Scope>(
                                                                                                                       Expressions::Scope(
                                                                                                                               scope)))));
        }

        return std::make_unique<Expressions::ListExpression>(
                Expressions::ListExpression(std::move(list), std::move(scope)));
    }

    throw std::invalid_argument("Expected string, found " + args[0]->toString());
}

expr_ptr listToStringFn(expression_vector args, scope_ptr scope)
{
    Functions::arg_count_check(args, 1);

    if (auto lst = dynamic_cast<Expressions::ListExpression *>(args[0].get()))
    {
        std::string str;

        for (auto &chrexp : lst->list)
        {
            if (auto chr = dynamic_cast<Expressions::CharacterExpression *>(chrexp.get()))
            {
                str.push_back(chr->character);
            }
            else throw std::invalid_argument("Expected character, found " + chrexp->toString());
        }

        return std::make_unique<Expressions::StringExpression>(Expressions::StringExpression(str, std::move(scope)));
    }

    throw std::invalid_argument("Expected string, found " + args[0]->toString());
}

expr_ptr charEqualFn(expression_vector args, scope_ptr scope)
{
    Functions::arg_count_check(args, 2);

    for (auto &arg : args)
    {
        if (arg->type() != "CharacterExpression")
            throw std::invalid_argument("Expected string, found " + arg->toString());
    }

    return std::make_unique<Expressions::BooleanValueExpression>
            (Expressions::BooleanValueExpression(
                    args[0]->toString() == args[1]->toString(),
                    std::move(scope)));
}

expr_ptr stringLTFunc(expression_vector args, scope_ptr scope)
{
    Functions::arg_count_check(args, 2);
    validate_string_fn(args);

    return std::make_unique<Expressions::BooleanValueExpression>
            (Expressions::BooleanValueExpression(
                    args[0]->toString() < args[1]->toString(),
                    std::move(scope)));
}

void register_string_functions()
{
    Functions::funcMap["string?"] = stringPredicateFunc;
    Functions::funcMap["string=?"] = stringEqualFunc;
    Functions::funcMap["string->symbol"] = stringToSymbolFunc;
    Functions::funcMap["string->int"] = stringToIntFunc;
    Functions::funcMap["int->string"] = intToStringFunc;
    Functions::funcMap["string->list"] = stringToListFn;
    Functions::funcMap["list->string"] = listToStringFn;
    Functions::funcMap["char=?"] = charEqualFn;
    Functions::funcMap["string<?"] = stringLTFunc;
}