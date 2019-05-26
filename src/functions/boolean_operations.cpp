//
// Created by Antonio Abbatangelo on 2019-05-11.
//

#include "functions.h"
#include "../parser.h"

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
                                      std::shared_ptr<Parser::Scope> scope) -> std::unique_ptr<Expressions::Expression>
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
                                                                                                std::shared_ptr<Parser::Scope>(
                                                                                                        new Parser::Scope(
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

void register_if_form()
{
    Functions::specialFormMap["if"] = [](Expressions::expression_vector expr,
                                         std::shared_ptr<Parser::Scope> scope) -> std::unique_ptr<Expressions::Expression>
    {
        if (expr.size() != 3) throw std::invalid_argument("if expects 3 args");

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
    };

    Functions::specialFormMap["cond"] = [](Expressions::expression_vector expr,
                                           std::shared_ptr<Parser::Scope> scope) -> std::unique_ptr<Expressions::Expression>
    {
        Expressions::expression_vector simplifiedExpr = Expressions::expression_vector();
        simplifiedExpr.insert(simplifiedExpr.begin(), std::move(Functions::getFormByName("cond", scope)));

        if (expr.empty()) throw std::invalid_argument("cond: Reached end without finding true condition");

        std::unique_ptr<Expressions::Expression> firstCond = Expressions::evaluate(std::move(expr[0]));
        std::unique_ptr<Expressions::Expression> tupleExpr = Expressions::evaluate(std::move(firstCond));

        if (auto tupleCast = dynamic_cast<Expressions::TupleExpression *>(tupleExpr.get()))
        {
            std::unique_ptr<Expressions::Expression> test = std::move(tupleCast->mTupleMembers[0]);

            if (!test->isValue())
            {
                test = Expressions::evaluate(std::move(test));
                tupleCast->mTupleMembers[0] = std::move(test);
                Parser::parseSpecialForm(tupleCast->toString(), scope, expr[0]);
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
        else throw std::invalid_argument("cond: Expected test, found " + tupleExpr->toString());

        for (auto &conditional : expr)
        {
            simplifiedExpr.insert(simplifiedExpr.end(), std::move(conditional));
        }

        return std::unique_ptr<Expressions::Expression>(new Expressions::TupleExpression(std::move(simplifiedExpr),
                                                                                         std::shared_ptr<Parser::Scope>(
                                                                                                 new Parser::Scope(
                                                                                                         scope))));
    };
}

void register_boolean_ops()
{
    register_relational_ops();
    register_if_form();

    using Expressions::expression_vector;

    Functions::funcMap["not"] = [](expression_vector expr,
                                   std::shared_ptr<Parser::Scope> scope) -> std::unique_ptr<Expressions::Expression>
    {
        if (expr.size() > 1) throw std::invalid_argument("not expects one argument"); //TODO: arg count

        if (auto bval = dynamic_cast<Expressions::BooleanValueExpression *>(expr[0].get()))
        {
            return std::unique_ptr<Expressions::Expression>(new Expressions::BooleanValueExpression(!bval->value,
                                                                                                    std::shared_ptr<Parser::Scope>(
                                                                                                            new Parser::Scope(
                                                                                                                    scope))));
        }
        else throw std::invalid_argument("not expected boolean, found: " + expr[0]->toString());
    };

    Functions::specialFormMap["and"] = [](expression_vector expr,
                                          std::shared_ptr<Parser::Scope> scope) -> std::unique_ptr<Expressions::Expression>
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

        return std::unique_ptr<Expressions::Expression>(new Expressions::BooleanValueExpression(retVal,
                                                                                                std::shared_ptr<Parser::Scope>(
                                                                                                        new Parser::Scope(
                                                                                                                scope))));
    };

    Functions::specialFormMap["or"] = [](expression_vector expr,
                                         std::shared_ptr<Parser::Scope> scope) -> std::unique_ptr<Expressions::Expression>
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

        return std::unique_ptr<Expressions::Expression>(new Expressions::BooleanValueExpression(retVal,
                                                                                                std::shared_ptr<Parser::Scope>(
                                                                                                        new Parser::Scope(
                                                                                                                scope))));
    };
}
