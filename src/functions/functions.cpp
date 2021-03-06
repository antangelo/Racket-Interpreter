//
// Created by Antonio on 2019-01-09.
//

#include "functions.h"
#include "../interpret/parser.h"
#include "../interpret/interpret.h"

void register_boolean_ops();

void register_math_functions();

void register_string_functions();

void register_symbol_functions();

void register_testing_functions();

void register_struct_functions();

void register_list_functions();

namespace Functions
{
    std::map<std::string, std::function<std::unique_ptr<Expressions::Expression>(expression_vector,
                                                                                 std::shared_ptr<Expressions::Scope>)>> funcMap;

    std::map<std::string, std::function<std::unique_ptr<Expressions::Expression>(expression_vector,
                                                                                 std::shared_ptr<Expressions::Scope>)>> specialFormMap;

    void arg_count_check(const expression_vector &args, int expectedCount)
    {
        if (args.size() != expectedCount)
            throw std::invalid_argument("Error: Expected " + std::to_string(expectedCount)
                                        + " argument(s), found " + std::to_string(args.size()) + ".");
    }

    std::unique_ptr<Expressions::Expression> display_func(expression_vector expr,
                                                          std::shared_ptr<Expressions::Scope> scope)
    {
        arg_count_check(expr, 1);

        std::cout << *expr.front();

        return std::unique_ptr<Expressions::Expression>(new Expressions::VoidValueExpression(
                std::make_unique<Expressions::Scope>(Expressions::Scope(std::move(scope)))));
    }

    std::unique_ptr<Expressions::Expression> newline_func(const expression_vector &expr,
                                                          std::shared_ptr<Expressions::Scope> scope)
    {
        arg_count_check(expr, 0);

        std::cout << std::endl;

        return std::unique_ptr<Expressions::Expression>(new Expressions::VoidValueExpression(
                std::make_unique<Expressions::Scope>(Expressions::Scope(std::move(scope)))));
    }

    std::unique_ptr<Expressions::Expression> begin_func(expression_vector expr,
                                                        const std::shared_ptr<Expressions::Scope> & /* scope */)
    {
        if (expr.empty()) throw std::invalid_argument("begin expects at least 1 arg, given none");

        for (int i = 0; i < expr.size() - 1; i++)
        {
            expr[i] = Expressions::evaluate(std::move(expr[i]));
        }

        return Expressions::evaluate(std::move(expr.back()));
    }

    std::unique_ptr<Expressions::Expression> define_lambda(const std::string &raw, expression_vector expr,
                                                           const std::shared_ptr<Expressions::Scope> &scope)
    {
        std::vector<std::string> fnSignature = Parser::parseTuple(raw);

        /** Pop the name off the signature, then the rest becomes the parameters of the function */
        std::string name = fnSignature[0];
        fnSignature.erase(fnSignature.begin());

        if (scope->contains(name) || funcMap.count(name) > 0 || specialFormMap.count(name) > 0)
            throw std::invalid_argument("Error: Scope already contains key for " + name);

        /** Next, we need to parse the lambda body */
        if (expr[1]->type() == "UnparsedExpression")
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
            lambdaBody.push_back(expr[1]->toString());

            auto binding = std::unique_ptr<Expressions::Expression>
                    (new Expressions::LambdaExpression(lambdaBody, fnSignature,
                                                       std::make_unique<Expressions::Scope>(
                                                               Expressions::Scope(scope))));

            // scope->parent is the tuple, we need to go further up to make our definition
            scope->parent->parent->define(name, std::move(binding));
            return std::unique_ptr<Expressions::Expression>
                    (new Expressions::VoidValueExpression(
                            std::make_unique<Expressions::Scope>(Expressions::Scope(scope))));
        }
        else
            throw std::invalid_argument(
                    "Error: Special form given parsed expression: " + expr[1]->toString());
    }

    std::unique_ptr<Expressions::Expression> define_form(expression_vector expr,
                                                         const std::shared_ptr<Expressions::Scope> &scope)
    {
        arg_count_check(expr, 2);

        std::string name;
        std::unique_ptr<Expressions::Expression> binding;

        if (expr[0]->type() == "UnparsedExpression")
        {
            name = expr[0]->toString();
            if (name.front() == '(' || name.front() == '[')
            {
                return define_lambda(name, std::move(expr), scope);
            }
        }
        else throw std::invalid_argument("Error: Special form given parsed expression: " + expr[0]->toString());

        if (scope->definitions.count(name) > 0 || funcMap.count(name) > 0 || specialFormMap.count(name) > 0)
            throw std::invalid_argument("Error: Scope already contains key for " + name);

        if (expr[1]->type() == "UnparsedExpression")
        {
            binding = Expressions::evaluate(std::move(expr[1]));
        }
        else throw std::invalid_argument("Error: Special form given parsed expression: " + expr[1]->toString());

        // scope->parent is the tuple, we need to go further up to make our definition
        scope->parent->parent->define(name, std::move(binding));

        return std::unique_ptr<Expressions::Expression>(
                new Expressions::VoidValueExpression(std::make_unique<Expressions::Scope>
                                                             (Expressions::Scope(scope))));
    }

    std::unique_ptr<Expressions::Expression> local_form(expression_vector args,
                                                        const std::shared_ptr<Expressions::Scope> &scope)
    {
        Functions::arg_count_check(args, 2);
        std::vector<std::string> definitionTuple = Parser::parseTuple(args[0]->toString());

        for (auto &definition : definitionTuple)
        {
            std::unique_ptr<Expressions::Expression> defExpr = Parser::parse(definition, scope->parent);
            Interpreter::interpret(std::move(defExpr));
        }

        return Expressions::evaluate(std::move(args[1]));
    }

    std::unique_ptr<Expressions::Expression> procedurePredicate(expression_vector args,
                                                                std::shared_ptr<Expressions::Scope> scope)
    {
        Functions::arg_count_check(args, 1);

        return std::make_unique<Expressions::BooleanValueExpression>
                (Expressions::BooleanValueExpression(args[0]->type() == "FunctionExpression", std::move(scope)));
    }

    std::unique_ptr<Expressions::Expression> equalComparator(expression_vector args,
                                                             std::shared_ptr<Expressions::Scope> scope)
    {
        Functions::arg_count_check(args, 2);

        return std::make_unique<Expressions::BooleanValueExpression>
                (args[0]->toString() == args[1]->toString(), std::move(scope));
    }

    std::unique_ptr<Expressions::Expression> error(const expression_vector &args,
                                                   const std::shared_ptr<Expressions::Scope> &/* scope */)
    {
        throw std::invalid_argument("Error: " + args[0]->toString());
    }

    std::unique_ptr<Expressions::Expression> identity(expression_vector args,
                                                      const std::shared_ptr<Expressions::Scope> &/* scope */)
    {
        arg_count_check(args, 1);
        return std::move(args[0]);
    }

    void registerFunctions()
    {
        specialFormMap["define"] = define_form;
        specialFormMap["local"] = local_form;

        register_math_functions();
        register_boolean_ops();
        register_string_functions();
        register_symbol_functions();
        register_testing_functions();
        register_struct_functions();
        register_list_functions();

        funcMap["display"] = display_func;
        funcMap["newline"] = newline_func;
        funcMap["begin"] = begin_func;
        funcMap["procedure?"] = procedurePredicate;
        funcMap["equal?"] = equalComparator;
        funcMap["error"] = error;
        funcMap["identity"] = identity;
    }

    std::unique_ptr<Expressions::Expression>
    getFormByName(const std::string &name, std::shared_ptr<Expressions::Scope> parent)
    {
        auto m = specialFormMap.at(name);
        std::shared_ptr<Expressions::Scope> localScope(new Expressions::Scope(std::move(parent)));

        return std::unique_ptr<Expressions::Expression>(
                new Expressions::SpecialFormExpression(name, m, std::move(localScope)));
    }

    std::unique_ptr<Expressions::Expression>
    getFuncByName(const std::string &name, std::shared_ptr<Expressions::Scope> parent)
    {
        auto m = funcMap.at(name);
        std::shared_ptr<Expressions::Scope> localScope(new Expressions::Scope(std::move(parent)));

        return std::unique_ptr<Expressions::Expression>(
                new Expressions::FunctionExpression(name, m, std::move(localScope)));
    }
}