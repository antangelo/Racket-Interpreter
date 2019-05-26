//
// Created by Antonio on 2019-01-09.
//

#include "../parser.h"

namespace Expressions
{
    std::ostream &operator<<(std::ostream &stream, const Expression &expr)
    {
        stream << expr.toString();
        return stream;
    }

    std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> obj_ref)
    {
        auto ref = obj_ref.get();
        return ref->evaluate(std::move(obj_ref));
    }

    /* UnparsedExpression */

    bool UnparsedExpression::isValue()
    {
        return true;
    }

    std::unique_ptr<Expression> UnparsedExpression::evaluate(std::unique_ptr<Expressions::Expression> /* obj_ref*/)
    {
        std::unique_ptr<Expression> expr;

        bool b = Parser::parse(mContents, localScope, expr);

        if (!b) throw std::invalid_argument("Parsing failed in expression: " + mContents);

        return std::move(expr);
    }

    std::string UnparsedExpression::toString() const
    {
        return mContents;
    }

    std::unique_ptr<Expression> UnparsedExpression::clone()
    {
        return std::unique_ptr<Expression>(new UnparsedExpression(*this, this->localScope));
    }

    /* NumericalValueExpression */

    bool NumericalValueExpression::isValue()
    {
        return true;
    }

    std::unique_ptr<Expression>
    NumericalValueExpression::evaluate(std::unique_ptr<Expression> obj_ref)
    {
        return std::move(obj_ref);
    }

    std::string NumericalValueExpression::toString() const
    {
        if (mValue.denominator() == 1) return std::to_string(mValue.numerator());

        return std::to_string(mValue.numerator()) + "/" + std::to_string(mValue.denominator());
    }

    std::unique_ptr<Expression> NumericalValueExpression::clone()
    {
        return std::unique_ptr<Expression>(new NumericalValueExpression(*this, this->localScope));
    }

    /* VoidValueExpression */

    bool VoidValueExpression::isValue()
    {
        return true;
    }

    std::unique_ptr<Expression>
    VoidValueExpression::evaluate(std::unique_ptr<Expressions::Expression> obj_ref)
    {
        return std::move(obj_ref);
    }

    std::string VoidValueExpression::toString() const
    {
        return "#:<void>";
    }

    std::unique_ptr<Expression> VoidValueExpression::clone()
    {
        return std::unique_ptr<Expression>(new VoidValueExpression(localScope));
    }

    /* BooleanValueExpression */

    bool BooleanValueExpression::isValue()
    {
        return true;
    }

    std::unique_ptr<Expression> BooleanValueExpression::evaluate(std::unique_ptr<Expressions::Expression> obj_ref)
    {
        return std::move(obj_ref);
    }

    std::string BooleanValueExpression::toString() const
    {
        return this->value ? "true" : "false";
    }

    std::unique_ptr<Expression> BooleanValueExpression::clone()
    {
        return std::unique_ptr<Expression>(new BooleanValueExpression(this->value, this->localScope));
    }
}
