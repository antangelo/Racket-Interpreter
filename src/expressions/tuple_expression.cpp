//
// Created by Antonio Abbatangelo on 2019-02-20.
//

#include "expressions.h"
#include "../interpret/parser.h"

namespace Expressions
{
    bool TupleExpression::isValue()
    {
        return false; // for now, will change in the future.
    }

    std::unique_ptr<Expression> TupleExpression::evaluate(std::unique_ptr<Expression> obj_ref)
    {
        for (auto &expr : mTupleMembers)
        {
            if (!expr->isValue())
            {
                expr = Expressions::evaluate(std::move(expr));
                return std::move(obj_ref); // Only evaluate one step
            }
        }

        if (auto func = dynamic_cast<FunctionExpression *>(mTupleMembers.front().get()))
        {
            // Need a new pointer in this scope to stop the function from dying when the first element is released.
            auto f = std::unique_ptr<Expression>(mTupleMembers.front().release());
            mTupleMembers.erase(mTupleMembers.begin());

            return func->call(std::move(mTupleMembers));
        }

        throw std::invalid_argument("Expected a function, found " + mTupleMembers.front()->toString());
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
        return std::unique_ptr<Expression>(new TupleExpression(*this, this->localScope));
    }
}