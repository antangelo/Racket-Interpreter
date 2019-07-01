//
// Created by Antonio Abbatangelo on 2019-06-15.
//

#include "functions.h"
#include "boost/multiprecision/cpp_int.hpp"

namespace StringFunctions
{
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
            Expressions::NumericalValueExpression::numerical_type num = arg->value;
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

            return std::make_unique<Expressions::StringExpression>(
                    Expressions::StringExpression(str, std::move(scope)));
        }

        throw std::invalid_argument("Expected string, found " + args[0]->toString());
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

    boost::multiprecision::cpp_int naturalFromRationalExpression(std::unique_ptr<Expressions::Expression> &expr)
    {
        if (auto rational = dynamic_cast<Expressions::NumericalValueExpression *>(expr.get()))
        {
            if (rational->value.denominator() == 1)
            {
                return rational->value.numerator();
            }
        }

        throw std::invalid_argument("Expected natural number, found " + expr->toString());
    }

    expr_ptr stringReplicateFn(expression_vector args, scope_ptr scope)
    {
        Functions::arg_count_check(args, 2);
        boost::multiprecision::cpp_int times = naturalFromRationalExpression(args[0]);

        if (auto str = dynamic_cast<Expressions::StringExpression *>(args[1].get()))
        {
            std::string rtn;
            for (boost::multiprecision::cpp_int i = 0; i < times; ++i)
            {
                rtn += str->str;
            }

            return std::make_unique<Expressions::StringExpression>(
                    Expressions::StringExpression(rtn, std::move(scope)));
        }
        else throw std::invalid_argument("Expected string, found " + args[1]->toString());
    }

    expr_ptr substringFn(expression_vector args, const scope_ptr &/* scope */)
    {
        if (args.size() < 2) throw std::invalid_argument("Expected 2 arguments, found " + std::to_string(args.size()));
        if (args.size() > 3) throw std::invalid_argument("Expected 3 arguments, found " + std::to_string(args.size()));

        bool threeArgSubstr = args.size() == 3;
        boost::multiprecision::cpp_int x = naturalFromRationalExpression(args[1]);
        boost::multiprecision::cpp_int y = 0;
        if (threeArgSubstr) y = naturalFromRationalExpression(args[2]);

        if (auto str = dynamic_cast<Expressions::StringExpression *>(args[0].get()))
        {
            if (threeArgSubstr) str->str = str->str.substr(x.convert_to<int>(), y.convert_to<int>());
            else str->str = str->str.substr(x.convert_to<int>());

            return std::move(args[0]);
        }
        else throw std::invalid_argument("Expected string, found " + args[0]->toString());
    }

    expr_ptr stringAppendFn(expression_vector args, scope_ptr scope)
    {
        std::string rtn;

        for (auto &expr : args)
        {
            if (auto str = dynamic_cast<Expressions::StringExpression *>(expr.get()))
            {
                rtn += str->str;
            }
            else throw std::invalid_argument("Expected string, found " + expr->toString());
        }

        return std::make_unique<Expressions::StringExpression>(Expressions::StringExpression(rtn, std::move(scope)));
    }
}

namespace CharacterFunctions
{
    typedef std::unique_ptr<Expressions::Expression> expr_ptr;
    typedef std::shared_ptr<Expressions::Scope> scope_ptr;
    using Expressions::expression_vector;

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

    expr_ptr charToIntFunc(expression_vector args, scope_ptr scope)
    {
        Functions::arg_count_check(args, 1);

        if (auto arg = dynamic_cast<Expressions::CharacterExpression *>(args[0].get()))
        {
            return std::make_unique<Expressions::NumericalValueExpression>
                    (Expressions::NumericalValueExpression(arg->character, std::move(scope)));
        }

        throw std::invalid_argument("Expected character, found " + args[0]->toString());
    }
}

void register_string_functions()
{
    Functions::funcMap["string?"] = StringFunctions::stringPredicateFunc;
    Functions::funcMap["string=?"] = StringFunctions::stringEqualFunc;
    Functions::funcMap["string->symbol"] = StringFunctions::stringToSymbolFunc;
    Functions::funcMap["string->int"] = StringFunctions::stringToIntFunc;
    Functions::funcMap["int->string"] = StringFunctions::intToStringFunc;
    Functions::funcMap["string->list"] = StringFunctions::stringToListFn;
    Functions::funcMap["list->string"] = StringFunctions::listToStringFn;
    Functions::funcMap["char=?"] = CharacterFunctions::charEqualFn;
    Functions::funcMap["string<?"] = StringFunctions::stringLTFunc;
    Functions::funcMap["replicate"] = StringFunctions::stringReplicateFn;
    Functions::funcMap["substring"] = StringFunctions::substringFn;
    Functions::funcMap["string-append"] = StringFunctions::stringAppendFn;
    Functions::funcMap["char->integer"] = CharacterFunctions::charToIntFunc;
}