//
// Created by Antonio Abbatangelo on 2019-05-11.
//

#include "functions.h"
#include "../interpret/parser.h"

bool compare(const std::string &comp, const boost::rational<int> &v1, const boost::rational<int> &v2)
{
    if (comp == "<")
        return v1 < v2;
    else if (comp == ">")
        return v1 > v2;
    else if (comp == "<=")
        return v1 <= v2;
    else if (comp == ">=")
        return v1 >= v2;
    else if (comp == "=")
        return v1 == v2;
    else
        throw std::invalid_argument("Invalid comparator " + comp);
}

void makeCompFunction(const std::string &comp)
{
    Functions::funcMap[comp] = [comp](Expressions::expression_vector expr,
                                      std::shared_ptr<Expressions::Scope> scope) -> std::unique_ptr<Expressions::Expression>
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
                retValue = retValue && compare(comp, compVal, ei->mValue);
                if (!retValue) break;

                compVal = ei->mValue;
            }
            else throw std::invalid_argument("Expected number, found " + expr[i]->toString());
        }

        return std::unique_ptr<Expressions::Expression>(new Expressions::BooleanValueExpression(retValue,
                                                                                                std::shared_ptr<Expressions::Scope>(
                                                                                                        new Expressions::Scope(
                                                                                                                scope))));
    };
}

void register_relational_ops()
{
    makeCompFunction("=");
    makeCompFunction("<");
    makeCompFunction(">");
    makeCompFunction("<=");
    makeCompFunction(">=");
}

std::unique_ptr<Expressions::Expression>
if_func(Expressions::expression_vector expr, const std::shared_ptr<Expressions::Scope> & /* scope */)
{
    Functions::arg_count_check(expr, 3);

    std::unique_ptr<Expressions::Expression> test = Expressions::evaluate(std::move(expr[0]));
    std::unique_ptr<Expressions::Expression> result;

    while (!test->isValue())
    {
        test = Expressions::evaluate(std::move(test));
    }

    if (auto boolVal = dynamic_cast<Expressions::BooleanValueExpression *>(test.get()))
    {
        if (boolVal->value)
        {
            result = Expressions::evaluate(std::move(expr[1]));
        }
        else
        {
            result = Expressions::evaluate(std::move(expr[2]));
        }
    }
    else throw std::invalid_argument("if expected boolean, found " + test->toString());

    return result;
}

std::unique_ptr<Expressions::Expression>
cond_func(Expressions::expression_vector expr, std::shared_ptr<Expressions::Scope> scope)
{
    Expressions::expression_vector simplifiedExpr = Expressions::expression_vector();
    simplifiedExpr.insert(simplifiedExpr.begin(), std::move(Functions::getFormByName("cond", scope)));

    if (expr.empty()) throw std::invalid_argument("cond: Reached end without finding true condition");

    std::unique_ptr<Expressions::Expression> firstCond = std::move(expr[0]);
    if (firstCond->type() == "UnparsedExpression") firstCond = Expressions::evaluate(std::move(firstCond));

    if (auto pex = dynamic_cast<Expressions::PartialExpression *>(firstCond.get()))
    {
        if (pex->mTupleMembers[0] == "else")
        {
            if (expr.size() > 1) throw std::invalid_argument("else clause must be last");

            pex->localScope->define("else", std::make_unique<Expressions::BooleanValueExpression>
                    (Expressions::BooleanValueExpression(true, nullptr)));
        }
    }

    if (firstCond->type() == "PartialExpression") firstCond = Expressions::evaluate(std::move(firstCond));

    if (auto tupleCast = dynamic_cast<Expressions::TupleExpression *>(firstCond.get()))
    {
        std::unique_ptr<Expressions::Expression> test = std::move(tupleCast->mTupleMembers[0]);

        if (!test->isValue())
        {
            test = Expressions::evaluate(std::move(test));
            tupleCast->mTupleMembers[0] = std::move(test);
            expr[0] = std::move(firstCond);
        }
        else
        {
            if (auto boolVal = dynamic_cast<Expressions::BooleanValueExpression *>(test.get()))
            {
                if (boolVal->value)
                {
                    return std::move(tupleCast->mTupleMembers[1]);
                }
                else
                {
                    expr.erase(expr.begin());
                }
            }
            else throw std::invalid_argument("cond: Expected boolean, found " + test->toString());
        }
    }
    else throw std::invalid_argument("cond: Expected test, found " + firstCond->toString());

    for (auto &conditional : expr)
    {
        simplifiedExpr.insert(simplifiedExpr.end(), std::move(conditional));
    }

    return std::unique_ptr<Expressions::Expression>(new Expressions::TupleExpression(std::move(simplifiedExpr),
                                                                                     std::shared_ptr<Expressions::Scope>(
                                                                                             new Expressions::Scope(
                                                                                                     std::move(
                                                                                                             scope)))));
}

std::unique_ptr<Expressions::Expression>
not_func(Expressions::expression_vector expr, std::shared_ptr<Expressions::Scope> scope)
{
    if (expr.size() > 1) throw std::invalid_argument("not expects one argument"); //TODO: arg count

    if (auto bval = dynamic_cast<Expressions::BooleanValueExpression *>(expr[0].get()))
    {
        return std::unique_ptr<Expressions::Expression>
                (new Expressions::BooleanValueExpression(!bval->value,
                                                         std::shared_ptr<Expressions::Scope>(
                                                                 new Expressions::Scope(
                                                                         std::move(scope)))));
    }
    else throw std::invalid_argument("not expected boolean, found: " + expr[0]->toString());
}

std::unique_ptr<Expressions::Expression>
and_func(Expressions::expression_vector expr, std::shared_ptr<Expressions::Scope> scope)
{
    bool retVal = true;

    for (auto &unparsedExpression : expr)
    {
        std::unique_ptr<Expressions::Expression> evaluated = Expressions::evaluate(std::move(unparsedExpression));

        while (!evaluated->isValue())
        {
            evaluated = Expressions::evaluate(std::move(evaluated));
        }

        if (auto bval = dynamic_cast<Expressions::BooleanValueExpression *>(evaluated.get()))
        {
            retVal = retVal && bval->value;
            if (!retVal) break;
        }
        else throw std::invalid_argument("and expected boolean, found: " + evaluated->toString());
    }

    return std::unique_ptr<Expressions::Expression>
            (new Expressions::BooleanValueExpression(retVal,
                                                     std::shared_ptr<Expressions::Scope>(
                                                             new Expressions::Scope(
                                                                     std::move(
                                                                             scope)))));
}

std::unique_ptr<Expressions::Expression>
or_func(Expressions::expression_vector expr, std::shared_ptr<Expressions::Scope> scope)
{
    bool retVal = false;

    for (auto &unparsedExpression : expr)
    {
        std::unique_ptr<Expressions::Expression> evaluated = Expressions::evaluate(std::move(unparsedExpression));

        while (!evaluated->isValue())
        {
            evaluated = Expressions::evaluate(std::move(evaluated));
        }

        if (auto bval = dynamic_cast<Expressions::BooleanValueExpression *>(evaluated.get()))
        {
            retVal = retVal || bval->value;
            if (retVal) break;
        }
        else throw std::invalid_argument("or expected boolean, found: " + evaluated->toString());
    }

    return std::unique_ptr<Expressions::Expression>
            (new Expressions::BooleanValueExpression(retVal,
                                                     std::shared_ptr<Expressions::Scope>(
                                                             new Expressions::Scope(
                                                                     std::move(scope)))));
}

void register_boolean_ops()
{
    register_relational_ops();

    Functions::specialFormMap["if"] = if_func;
    Functions::specialFormMap["cond"] = cond_func;

    Functions::funcMap["not"] = not_func;
    Functions::specialFormMap["and"] = and_func;
    Functions::specialFormMap["or"] = or_func;
}
