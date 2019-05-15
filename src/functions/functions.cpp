//
// Created by Antonio on 2019-01-09.
//

#include "functions.h"
#include "../parser.h"

/* From boolean_operations.cpp */
void register_boolean_ops();

namespace Functions
{
    std::map<std::string, std::function<std::unique_ptr<Expressions::Expression>(expression_vector,
                                                                                 Parser::Scope *)>> funcMap;

    std::map<std::string, std::function<std::unique_ptr<Expressions::Expression>(expression_vector,
                                                                                 Parser::Scope *)>> specialFormMap;

    void registerSpecialForms()
    {
        specialFormMap["define"] = [](expression_vector expr,
                                      Parser::Scope *scope) -> std::unique_ptr<Expressions::Expression>
        {
            std::string name;
            std::unique_ptr<Expressions::Expression> binding;

            //TODO: tidy up this code

            if (auto raw = dynamic_cast<Expressions::UnparsedExpression *>(expr[0].get()))
            {
                if (raw->toString().front() == '(' || raw->toString().front() == '[')
                {
                    std::vector<std::string> fnSignature = Parser::parseTuple(raw->toString());

                    /** Pop the name off the signature, then the rest becomes the parameters of the function */
                    name = fnSignature[0];
                    fnSignature.erase(fnSignature.begin());

                    /** Next, we need to parse the lambda body */
                    if (auto body = dynamic_cast<Expressions::UnparsedExpression *>(expr[1].get()))
                    {
                        /** The lambda requires a string containing the args to be given to it */
                        std::string lambdaArgs = "(";
                        for (int i = 0; i < fnSignature.size(); ++i)
                        {
                            lambdaArgs += fnSignature[i];
                            if (i < fnSignature.size() - 1) lambdaArgs += " ";
                        }
                        lambdaArgs += ")";

                        /** Formatting for the lambda expression */
                        std::vector<std::string> lambdaBody;
                        lambdaBody.emplace_back("lambda");
                        lambdaBody.push_back(lambdaArgs);
                        lambdaBody.push_back(body->toString());

                        binding = std::unique_ptr<Expressions::Expression>
                                (new Expressions::LambdaExpression(lambdaBody, fnSignature));

                        scope->define(name, std::move(binding));
                        return std::unique_ptr<Expressions::Expression>(new Expressions::VoidValueExpression());
                    }
                    else
                        throw std::invalid_argument(
                                "Error: Special form given parsed expression: " + expr[1]->toString());
                }

                name = raw->toString();
            }
            else throw std::invalid_argument("Error: Special form given parsed expression: " + expr[0]->toString());

            if (auto raw = dynamic_cast<Expressions::UnparsedExpression *>(expr[1].get()))
            {
                binding = raw->evaluate(std::move(expr[1]), scope);
            }
            else throw std::invalid_argument("Error: Special form given parsed expression: " + expr[1]->toString());

            scope->define(name, std::move(binding));

            return std::unique_ptr<Expressions::Expression>(new Expressions::VoidValueExpression());
        };
    }

    void registerFunctions()
    {
        registerSpecialForms();
        register_boolean_ops();

        funcMap["+"] = [](expression_vector expr, Parser::Scope *scope) -> std::unique_ptr<Expressions::Expression>
        {
            boost::rational<int> sum = 0;

            for (auto &i : expr)
            {
                if (auto num = dynamic_cast<Expressions::NumericalValueExpression *>(i.get()))
                {
                    sum += num->mValue;
                } else throw std::invalid_argument("+ expects numerical arg, found: " + i->toString());
            }

            return std::unique_ptr<Expressions::Expression>(new Expressions::NumericalValueExpression(sum));
        };

        funcMap["*"] = [](expression_vector expr, Parser::Scope *) -> std::unique_ptr<Expressions::Expression>
        {
            boost::rational<int> product(1);

            for (auto &i : expr)
            {
                if (auto num = dynamic_cast<Expressions::NumericalValueExpression *>(i.get()))
                {
                    product *= num->mValue;
                } else throw std::invalid_argument("* expects numerical arg, found: " + i->toString());
            }

            return std::unique_ptr<Expressions::Expression>(new Expressions::NumericalValueExpression(product));
        };

        funcMap["-"] = [](expression_vector expr, Parser::Scope *) -> std::unique_ptr<Expressions::Expression>
        {
            if (expr.empty()) throw std::invalid_argument("- expected at least 1 arg.");

            if (auto first = dynamic_cast<Expressions::NumericalValueExpression *>(expr.front().get()))
            {
                if (expr.size() == 1)
                {
                    return std::unique_ptr<Expressions::Expression>
                            (new Expressions::NumericalValueExpression(-1 * first->mValue));
                }

                boost::rational<int> diff = first->mValue;

                for (int i = 1; i < expr.size(); i++)
                {
                    auto &j = expr[i];
                    if (auto num = dynamic_cast<Expressions::NumericalValueExpression *>(j.get()))
                    {
                        diff -= num->mValue;
                    } else throw std::invalid_argument("- expects numerical arg, found: " + j->toString());
                }

                return std::unique_ptr<Expressions::Expression>(new Expressions::NumericalValueExpression(diff));
            } else throw std::invalid_argument("- expects numerical arg, found: " + expr.front()->toString());
        };

        funcMap["/"] = [](expression_vector expr, Parser::Scope *) -> std::unique_ptr<Expressions::Expression>
        {
            if (expr.empty()) throw std::invalid_argument("/ expected at least 1 arg.");

            if (auto first = dynamic_cast<Expressions::NumericalValueExpression *>(expr.front().get()))
            {
                if (expr.size() == 1)
                {
                    return std::unique_ptr<Expressions::Expression>
                            (new Expressions::NumericalValueExpression(1 / first->mValue));
                }

                boost::rational<int> quotient = first->mValue;

                for (int i = 1; i < expr.size(); i++)
                {
                    auto &j = expr[i];
                    if (auto num = dynamic_cast<Expressions::NumericalValueExpression *>(j.get()))
                    {
                        quotient /= num->mValue;
                    } else throw std::invalid_argument("/ expects numerical arg, found: " + j->toString());
                }

                return std::unique_ptr<Expressions::Expression>(new Expressions::NumericalValueExpression(quotient));
            } else throw std::invalid_argument("/ expects numerical arg, found: " + expr.front()->toString());
        };

        funcMap["display"] = [](expression_vector expr, Parser::Scope *) -> std::unique_ptr<Expressions::Expression>
        {
            if (expr.empty()) throw std::invalid_argument("display expects at least 1 arg.");

            std::cout << *expr.front();

            return std::unique_ptr<Expressions::Expression>(new Expressions::VoidValueExpression());
        };

        funcMap["newline"] = [](expression_vector expr, Parser::Scope *) -> std::unique_ptr<Expressions::Expression>
        {
            if (!expr.empty()) throw std::invalid_argument("newline expects no args");

            std::cout << std::endl;

            return std::unique_ptr<Expressions::Expression>(new Expressions::VoidValueExpression());
        };

        funcMap["begin"] = [](expression_vector expr, Parser::Scope *scope) -> std::unique_ptr<Expressions::Expression>
        {
            if (expr.empty()) throw std::invalid_argument("begin expects at least 1 arg.");

            for (int i = 0; i < expr.size() - 1; i++)
            {
                expr[i] = Expressions::evaluate(std::move(expr[i]), scope);
            }

            return Expressions::evaluate(std::move(expr.back()), scope);
        };
    }

    std::unique_ptr<Expressions::Expression> getFormByName(const std::string &name)
    {
        auto m = specialFormMap.at(name);

        return std::unique_ptr<Expressions::Expression>(new Expressions::SpecialFormExpression(name, m));
    }

    std::unique_ptr<Expressions::Expression> getFuncByName(const std::string &name)
    {
        auto m = funcMap.at(name);

        return std::unique_ptr<Expressions::Expression>(new Expressions::FunctionExpression(name, m));
    }
}