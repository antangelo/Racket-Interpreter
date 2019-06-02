//
// Created by Antonio on 2019-01-09.
//

#include "../interpret/parser.h"

namespace Expressions
{
    bool Scope::contains(const std::string &token)
    {
        return definitions.find(token) != definitions.end()
               || (parent != nullptr && parent->contains(token));
    }

    void Scope::define(const std::string &key, std::unique_ptr<Expressions::Expression> val)
    {
        definitions[key] = std::move(val);
    }

    std::unique_ptr<Expressions::Expression> Scope::getDefinition(const std::string &key)
    {
        if (definitions.find(key) != definitions.end())
        {
            // Don't want to give the definition itself, only a copy of it
            return definitions[key]->clone();
        }
        else if (parent != nullptr && parent->contains(key))
        {
            return parent->getDefinition(key);
        }
        else throw std::invalid_argument("Key " + key + " not found in scope."); //TODO: replace invalid_argument
    }

    void Scope::defineGlobal(const std::string &key, std::unique_ptr<Expressions::Expression> val)
    {
        if (!this->globalScope) this->define(key, std::move(val));
        else this->globalScope->define(key, std::move(val));
    }

    std::string Scope::toString()
    {
        std::string str;

        for (auto &def : definitions)
        {
            str += " ";
            str += def.first;
            str += ": ";
            str += def.second->toString();
        }

        if (parent != nullptr) str += parent->toString();

        return std::move(str);
    }

    /* Expression */

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
        auto expr = Parser::parse(mContents, localScope);
        if (!expr) throw std::invalid_argument("Parsing failed in expression: " + mContents);

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
