//
// Created by Antonio Abbatangelo on 2019-02-20.
//

#include "expressions.h"
#include "../parser.h"
#include "../functions/functions.h"

namespace Expressions
{

    bool PartialExpression::isValue()
    {
        return false;
    }

    std::unique_ptr<Expression>
    PartialExpression::evaluate(std::unique_ptr<Expression> /* obj_ref */, Parser::Scope *scope)
    {
        Expressions::expression_vector members;

        if (mTupleMembers.front() == "lambda")
        {
            /** This parses the tuple containing the parameters of the lambda */
            std::vector<std::string> params = Parser::parseTuple(mTupleMembers.at(1));

            return std::unique_ptr<Expression>(new LambdaExpression(mTupleMembers, params));
        }
        else if (Functions::specialFormMap.find(mTupleMembers.front()) != Functions::specialFormMap.end())
        {
            /** We want to give the special form an unevaluated TupleExpression */

            for (const auto &str : mTupleMembers)
            {
                std::unique_ptr<Expressions::Expression> expr;
                Parser::parseSpecialForm(str, expr);

                members.push_back(std::move(expr));
            }

            // std::make_unique doesn't work here since it calls TupleExpression's copy constructor for some reason
            std::unique_ptr<TupleExpression> expr(new TupleExpression(std::move(members)));
            return expr;
        }

        for (const auto &str : mTupleMembers)
        {
            std::unique_ptr<Expressions::Expression> expr;
            auto parseSuccessful = Parser::parse(str, scope, expr);

            if (!parseSuccessful)
                throw std::invalid_argument(
                        "Parsing failed"); //TODO: Write exception for syntax errors/unsuccessful parsing.

            if (auto partialExpression = dynamic_cast<PartialExpression *>(expr.get()))
            {
                // Not the best, but obj_ref isn't used by evaluate() so it ends up saving
                // unnecessary pointer creation with a PartialExpression here.
                members.push_back(std::move(partialExpression->evaluate(std::move(expr), scope)));
            }
            else
            {
                members.push_back(std::move(expr));
            }
        }

        std::unique_ptr<TupleExpression> expr(new TupleExpression(std::move(members)));
        return std::move(expr);
    }

    std::string PartialExpression::toString() const
    {
        std::string str = "(";

        for (const auto &i : mTupleMembers)
        {
            str += i + " ";
        }

        return str.substr(0, str.size() - 1) + ")";
    }

    std::unique_ptr<Expression> PartialExpression::clone()
    {
        return std::unique_ptr<Expression>(new PartialExpression(*this));
    }
}