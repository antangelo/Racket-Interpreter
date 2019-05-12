//
// Created by Antonio Abbatangelo on 2019-05-11.
//

#include "functions.h"

void register_relational_ops()
{
    using Expressions::expression_vector;

    Functions::funcMap["="] = [](expression_vector expr,
                                 Parser::Scope *scope) -> std::unique_ptr<Expressions::Expression>
    {
        if (expr.size() < 2) throw std::invalid_argument("Expected at least two arguments"); //TODO: arg count
        boost::rational<int> compVal;
        bool retValue = true;

        if (auto e1 = dynamic_cast<Expressions::NumericalValueExpression *>(expr[0].get()))
        {
            compVal = e1->mValue;
        }
        else throw std::invalid_argument("Expected number, found " + expr[0]->toString());

        for (int i = 1; i < expr.size(); ++i)
        {
            if (auto ei = dynamic_cast<Expressions::NumericalValueExpression *>(expr[i].get()))
            {
                retValue = retValue && (ei->mValue == compVal);
                if (!retValue) break;
            }
            else throw std::invalid_argument("Expected number, found " + expr[i]->toString());
        }

        return std::unique_ptr<Expressions::Expression>(new Expressions::BooleanValueExpression(retValue));
    };
}

void register_boolean_ops()
{
    register_relational_ops();

    using Expressions::expression_vector;

    Functions::funcMap["not"] = [](expression_vector expr,
                                   Parser::Scope *scope) -> std::unique_ptr<Expressions::Expression>
    {
        if (expr.size() > 1) throw std::invalid_argument("not expects one argument"); //TODO: arg count

        if (auto bval = dynamic_cast<Expressions::BooleanValueExpression *>(expr[0].get()))
        {
            return std::unique_ptr<Expressions::Expression>(new Expressions::BooleanValueExpression(!bval->value));
        }
        else throw std::invalid_argument("not expected boolean, found: " + expr[0]->toString());
    };

    Functions::specialFormMap["and"] = [](expression_vector expr,
                                          Parser::Scope *scope) -> std::unique_ptr<Expressions::Expression>
    {
        bool retVal = true;

        for (auto &exp : expr)
        {
            auto ref = exp.get();
            std::unique_ptr<Expressions::Expression> evaluated = ref->evaluate(std::move(exp), scope);

            if (auto bval = dynamic_cast<Expressions::BooleanValueExpression *>(evaluated.get()))
            {
                retVal = retVal && bval->value;
                if (!retVal) break;
            }
            else throw std::invalid_argument("and expected boolean, found: " + evaluated->toString());
        }

        return std::unique_ptr<Expressions::Expression>(new Expressions::BooleanValueExpression(retVal));
    };

    Functions::specialFormMap["or"] = [](expression_vector expr,
                                         Parser::Scope *scope) -> std::unique_ptr<Expressions::Expression>
    {
        bool retVal = false;

        for (auto &exp : expr)
        {
            auto ref = exp.get();
            std::unique_ptr<Expressions::Expression> evaluated = ref->evaluate(std::move(exp), scope);

            if (auto bval = dynamic_cast<Expressions::BooleanValueExpression *>(evaluated.get()))
            {
                retVal = retVal || bval->value;
                if (retVal) break;
            }
            else throw std::invalid_argument("or expected boolean, found: " + evaluated->toString());
        }

        return std::unique_ptr<Expressions::Expression>(new Expressions::BooleanValueExpression(retVal));
    };
}
