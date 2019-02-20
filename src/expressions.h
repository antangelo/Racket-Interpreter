//
// Created by Antonio on 2019-01-09.
//

#ifndef RACKET_INTERPRETER_EXPRESSIONS_H
#define RACKET_INTERPRETER_EXPRESSIONS_H

#include <iostream>
#include <string>
#include <vector>
#include <functional>

namespace Parser
{
    class Scope;
}

namespace Expressions
{
    class Expression
    {
    public:
        /* Should return true if the expression is a value, that is,
         * if it can't be simplified any further. */
        virtual bool isValue() = 0;

        /* Should evaluate the expression and produce an equivalent expression that is
         * one step closer to being a value. If the expression is a value, should produce itself. */
        virtual std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> *obj_ref, Parser::Scope *scope) = 0;

        virtual std::string toString() const = 0;

        virtual std::unique_ptr<Expression> clone() = 0;

        friend std::ostream &operator<<(std::ostream &stream, const Expression &expr);

        virtual ~Expression() = default;

    protected:
        explicit Expression() = default;
    };

    typedef std::vector<std::unique_ptr<Expression>> expression_vector;

    class PartialExpression : public Expression
    {
    public:
        std::vector<std::string> mTupleMembers;

        bool isValue() override;

        std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> *obj_ref, Parser::Scope *scope) override;

        std::string toString() const override;

        std::unique_ptr<Expression> clone() override;

        explicit PartialExpression(std::vector<std::string> tuple)
        {
            mTupleMembers = std::move(tuple);
        }

    private:
        PartialExpression(const PartialExpression &old_expr)
        {
            this->mTupleMembers = std::vector<std::string>(old_expr.mTupleMembers);
        }
    };

    class TupleExpression : public Expression
    {
    public:
        expression_vector mTupleMembers;

        bool isValue() override;

        std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> *obj_ref, Parser::Scope *scope) override;

        std::string toString() const override;

        std::unique_ptr<Expression> clone() override;

        explicit TupleExpression(expression_vector tuple)
        {
            mTupleMembers = std::move(tuple); //TODO: Re-implement to avoid double move call
        }

    private:
        TupleExpression(const TupleExpression &old_expr)
        {
            for (auto &exp : old_expr.mTupleMembers)
            {
                this->mTupleMembers.push_back(exp->clone());
            }
        }
    };

    class FunctionExpression : public Expression
    {
    public:
        bool isValue() override;

        std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> *obj_ref, Parser::Scope *scope) override;

        std::string toString() const override;

        std::unique_ptr<Expression> clone() override;

        virtual std::unique_ptr<Expression> call(expression_vector args, Parser::Scope *scope);

        explicit FunctionExpression(std::string funcName,
                                    std::function<std::unique_ptr<Expression>(expression_vector,
                                                                              Parser::Scope *)> &func)
        {
            mFuncName = std::move(funcName);
            mFunction = func;
        }

    protected:
        explicit FunctionExpression() = default;
        std::string mFuncName;
        std::function<std::unique_ptr<Expression>(expression_vector, Parser::Scope *)> mFunction;

    private:
        FunctionExpression(const FunctionExpression &old_expr)
        {
            this->mFuncName = old_expr.mFuncName;
        }
    };

    class LambdaExpression : public FunctionExpression
    {
    public:
        std::unique_ptr<Expression> call(expression_vector args, Parser::Scope *scope) override;

        /* lambda_expr should have "lambda" at front(), but it should have the arg tuple */
        explicit LambdaExpression(std::vector<std::string> lambda_expr, std::vector<std::string> lambda_args)
        {
            if (lambda_expr.size() < 3)
                throw std::invalid_argument("Lambda requires two params"); //TODO: more descriptive

            mFuncName = "(lambda ";

            for (int i = 1; i < lambda_expr.size(); i++)
            {
                mFuncName += lambda_expr[i];

                if (i < lambda_expr.size() - 1) mFuncName += " ";
            }

            mFuncName += ")";

            mLambdaArgs = std::move(lambda_args);
            mLambdaExpr = std::move(lambda_expr);
        }

    protected:
        std::vector<std::string> mLambdaArgs;
        std::vector<std::string> mLambdaExpr;
    };

    class NumericalValueExpression : public Expression // TODO: Implement non-numerical values.
    {
    public:
        double mValue;

        bool isValue() override;

        std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> *obj_ref, Parser::Scope *scope) override;

        std::string toString() const override;

        std::unique_ptr<Expression> clone() override;

        explicit NumericalValueExpression(double value)
        {
            mValue = value;
        }

    private:
        NumericalValueExpression(const NumericalValueExpression &old_expr)
        {
            this->mValue = old_expr.mValue;
        }
    };

    class VoidValueExpression : public Expression
    {
    public:
        bool isValue() override;

        std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> *obj_ref, Parser::Scope *scope) override;

        std::string toString() const override;

        std::unique_ptr<Expression> clone() override;
    };
}

#endif //RACKET_INTERPRETER_EXPRESSIONS_H
