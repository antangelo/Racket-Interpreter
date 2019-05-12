//
// Created by Antonio Abbatangelo on 2019-02-20.
//

#include "expressions.h"
#include "../parser.h"

namespace Expressions
{

/* FunctionExpression */
    inline bool FunctionExpression::isValue()
    {
        return true;
    }

    inline std::unique_ptr<Expression>
    FunctionExpression::evaluate(std::unique_ptr<Expressions::Expression> obj_ref, Parser::Scope * /* scope */)
    {
        return std::move(obj_ref);
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
}