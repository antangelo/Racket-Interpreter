//
// Created by Antonio on 2019-01-09.
//

#include "parser.h"
#include "functions.h"

namespace Expressions
{
    std::ostream &operator<<(std::ostream &stream, const Expression &expr)
    {
        stream << expr.toString();
        return stream;
    }

    /* UnparsedExpression */

    bool UnparsedExpression::isValue()
    {
        return true;
    }

    std::unique_ptr<Expression> UnparsedExpression::evaluate(std::unique_ptr<Expressions::Expression> *obj_ref,
                                                             Parser::Scope *scope)
    {
        std::unique_ptr<Expression> expr;

        bool b = Parser::parse(mContents, scope, expr);

        if (!b) throw std::invalid_argument("Parsing failed in expression: " + mContents);

        return std::move(expr);
    }

    std::string UnparsedExpression::toString() const
    {
        return mContents;
    }

    std::unique_ptr<Expression> UnparsedExpression::clone()
    {
        return std::unique_ptr<Expression>(new UnparsedExpression(*this));
    }

    /* PartialExpression */

    bool PartialExpression::isValue()
    {
        return false;
    }

    std::unique_ptr<Expression> PartialExpression::evaluate(std::unique_ptr<Expression> *obj_ref, Parser::Scope *scope)
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
                Parser::parseSpecialForm(str, scope, expr);

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
                members.push_back(std::move(partialExpression->evaluate(obj_ref, scope)));
            } else
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

    /* TupleExpression */

    bool TupleExpression::isValue()
    {
        return false; // for now, will change in the future.
    }

    std::unique_ptr<Expression> TupleExpression::evaluate(std::unique_ptr<Expression> *obj_ref, Parser::Scope *scope)
    {
        for (auto &expr : mTupleMembers)
        {
            if (!expr->isValue())
            {
                expr = expr->evaluate(&expr, scope);
                return std::move(*obj_ref);
            }
        }

        if (auto func = dynamic_cast<FunctionExpression *>(mTupleMembers.front().get()))
        {
            // Need a new pointer in this scope to stop the function from dying when the first element is released.
            auto f = std::unique_ptr<Expression>(mTupleMembers.front().release());
            mTupleMembers.erase(mTupleMembers.begin());

            return func->call(std::move(mTupleMembers), scope);
        }

        return std::move(*obj_ref);
    }

    std::string TupleExpression::toString() const
    {
        std::string str = "(";

        for (const auto &i : mTupleMembers)
        {
            str += i->toString() + " ";
        }

        return str.substr(0, str.size() - 1) + ")";
    }

    std::unique_ptr<Expression> TupleExpression::clone()
    {
        return std::unique_ptr<Expression>(new TupleExpression(*this));
    }

    /* FunctionExpression */
    inline bool FunctionExpression::isValue()
    {
        return true;
    }

    inline std::unique_ptr<Expression>
    FunctionExpression::evaluate(std::unique_ptr<Expressions::Expression> *obj_ref, Parser::Scope *scope)
    {
        return std::move(*obj_ref);
    }

    std::string FunctionExpression::toString() const
    {
        return mFuncName;
    }

    bool FunctionExpression::isSpecialForm()
    {
        return mSpecialForm;
    }

    std::unique_ptr<Expression> FunctionExpression::call(Expressions::expression_vector args, Parser::Scope *scope)
    {
        return mFunction(std::move(args), scope);
    }

    std::unique_ptr<Expression> FunctionExpression::clone()
    {
        return std::unique_ptr<Expression>(new FunctionExpression(*this));
    }

    /* LambdaExpression */

    std::unique_ptr<Expression> LambdaExpression::call(Expressions::expression_vector args, Parser::Scope *scope)
    {
        if (mLambdaArgs.size() != args.size()) throw std::invalid_argument("Lambda arg parity mismatch");
        std::string toParser;

        Parser::Scope fnScope(scope);

        for (int i = 0; i < args.size(); ++i)
        {
            /** Move all arguments into the function's scope */
            fnScope.define(mLambdaArgs[i], std::move(args[i]));
        }

        for (int i = 2; i < mLambdaExpr.size(); ++i)
        {
            toParser += mLambdaExpr[i];
        }

        std::unique_ptr<Expression> expr;
        bool success = Parser::parse(toParser, &fnScope, expr);

        if (!success) throw std::invalid_argument("Parsing failed in lambda: " + toString());

        if (auto pex = dynamic_cast<PartialExpression *>(expr.get()))
        {
            // Evaluate the resulting partial expression in the scope of the function.
            expr = pex->evaluate(nullptr, &fnScope);
        }

        return expr;
    }

    std::unique_ptr<Expression> LambdaExpression::clone()
    {
        return std::unique_ptr<Expression>(new LambdaExpression(*this));
    }

    /* NumericalValueExpression */

    bool NumericalValueExpression::isValue()
    {
        return true;
    }

    std::unique_ptr<Expression>
    NumericalValueExpression::evaluate(std::unique_ptr<Expression> *obj_ref, Parser::Scope *scope)
    {
        return std::move(*obj_ref);
    }

    std::string NumericalValueExpression::toString() const
    {
        return std::to_string(mValue);
    }

    std::unique_ptr<Expression> NumericalValueExpression::clone()
    {
        return std::unique_ptr<Expression>(new NumericalValueExpression(*this));
    }

    /* VoidValueExpression */

    bool VoidValueExpression::isValue()
    {
        return true;
    }

    std::unique_ptr<Expression>
    VoidValueExpression::evaluate(std::unique_ptr<Expressions::Expression> *obj_ref, Parser::Scope *scope)
    {
        return std::move(*obj_ref);
    }

    std::string VoidValueExpression::toString() const
    {
        return "#:<void>";
    }

    std::unique_ptr<Expression> VoidValueExpression::clone()
    {
        return std::unique_ptr<Expression>(new VoidValueExpression());
    }
}
