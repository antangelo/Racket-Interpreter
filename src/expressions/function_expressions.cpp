//
// Created by Antonio Abbatangelo on 2019-02-20.
//

#include "expressions.h"
#include "../interpret/parser.h"

namespace Expressions
{

/* FunctionExpression */

    bool FunctionExpression::isValue()
    {
        return true;
    }

    std::unique_ptr<Expression>
    FunctionExpression::evaluate(std::unique_ptr<Expression> obj_ref)
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

    std::unique_ptr<Expression> FunctionExpression::call(expression_vector args)
    {
        return mFunction(std::move(args), localScope);
    }

    std::unique_ptr<Expression> FunctionExpression::clone()
    {
        return std::unique_ptr<Expression>(new FunctionExpression(*this, this->localScope));
    }

/* LambdaExpression */

    std::unique_ptr<Expression> LambdaExpression::call(expression_vector args)
    {
        if (mLambdaArgs.size() != args.size()) throw std::invalid_argument("Lambda arg parity mismatch");
        std::string toParser;

        std::shared_ptr<Expressions::Scope> fnScope(new Expressions::Scope(localScope));

        for (int i = 0; i < args.size(); ++i)
        {
            /** Move all arguments into the function's scope */
            fnScope->define(mLambdaArgs[i], std::move(args[i]));
        }

        for (int i = 2; i < mLambdaExpr.size(); ++i)
        {
            toParser += mLambdaExpr[i];
        }

        auto expr = Parser::parse(toParser, fnScope);

        if (!expr) throw std::invalid_argument("Parsing failed in lambda: " + toString());

        if (auto pex = dynamic_cast<PartialExpression *>(expr.get()))
        {
            // Evaluate the resulting partial expression in the scope of the function.
            expr = pex->evaluate(std::move(expr));
        }

        return expr;
    }

    std::unique_ptr<Expression> LambdaExpression::clone()
    {
        return std::unique_ptr<Expression>(new LambdaExpression(*this, this->localScope));
    }
}