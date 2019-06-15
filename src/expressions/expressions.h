//
// Created by Antonio on 2019-01-09.
//

#ifndef RACKET_INTERPRETER_EXPRESSIONS_H
#define RACKET_INTERPRETER_EXPRESSIONS_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <map>

#include "boost/rational.hpp"

namespace Expressions
{
    class Expression;

    class Scope
    {
    public:
        explicit Scope(std::shared_ptr<Scope> parent)
        {
            this->parent = std::move(parent);

            if (this->parent && this->parent->globalScope) this->globalScope = this->parent->globalScope;
            else this->globalScope = this->parent.get();
        }

        bool contains(const std::string &);

        void define(const std::string &, std::unique_ptr<Expressions::Expression>);

        void defineGlobal(const std::string &, std::unique_ptr<Expressions::Expression>);

        void clear();

        std::unique_ptr<Expressions::Expression> getDefinition(const std::string &);

        std::string toString();

    public:
        std::map<std::string, std::unique_ptr<Expressions::Expression>> definitions;
        Scope *globalScope;

    private:
        std::shared_ptr<Scope> parent;
    };

    class Expression
    {
    public:
        /* Should return true if the expression is a value, that is,
         * if it can't be simplified any further. */
        virtual bool isValue() = 0;

        /* Should evaluate the expression and produce an equivalent expression that is
         * one step closer to being a value. If the expression is a value, should produce itself (obj_ref). */
        virtual std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> obj_ref) = 0;

        virtual std::string toString() const = 0;

        virtual std::string type() const final;

        virtual std::unique_ptr<Expression> clone() = 0;

        friend std::ostream &operator<<(std::ostream &stream, const Expression &expr);

        virtual ~Expression() = default;

        std::shared_ptr<Scope> localScope;

    protected:
        explicit Expression(std::shared_ptr<Scope> scope, const std::string &exprType)
        {
            this->localScope = std::move(scope);
            this->exprType = exprType;
        }

    private:
        std::string exprType;
    };


    std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression>);

    typedef std::vector<std::unique_ptr<Expression>> expression_vector;

    class UnparsedExpression : public Expression
    {
    public:
        std::string mContents;

        bool isValue() override;

        std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> obj_ref) override;

        std::string toString() const override;

        std::unique_ptr<Expression> clone() override;

        explicit UnparsedExpression(std::string contents, std::unique_ptr<Scope> scope)
                : Expression(std::move(scope), "UnparsedExpression")
        {
            this->mContents = std::move(contents);
        }

    private:
        UnparsedExpression(const UnparsedExpression &old_expr, std::shared_ptr<Scope> scope)
                : Expression(std::move(scope), "UnparsedExpression")
        {
            this->mContents = old_expr.mContents;
        }
    };

    class PartialExpression : public Expression
    {
    public:
        std::vector<std::string> mTupleMembers;

        bool isValue() override;

        std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> obj_ref) override;

        std::string toString() const override;

        std::unique_ptr<Expression> clone() override;

        explicit PartialExpression(std::vector<std::string> tuple, std::shared_ptr<Scope> scope)
                : Expression(std::move(scope), "PartialExpression")
        {
            mTupleMembers = std::move(tuple);
        }

    private:
        PartialExpression(const PartialExpression &old_expr, std::shared_ptr<Scope> scope)
                : Expression(std::move(scope), "PartialExpression")
        {
            this->mTupleMembers = std::vector<std::string>(old_expr.mTupleMembers);
        }
    };

    class TupleExpression : public Expression
    {
    public:
        expression_vector mTupleMembers;

        bool isValue() override;

        std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> obj_ref) override;

        std::string toString() const override;

        std::unique_ptr<Expression> clone() override;

        explicit TupleExpression(expression_vector tuple, std::shared_ptr<Scope> scope)
                : Expression(std::move(scope), "TupleExpression")
        {
            mTupleMembers = std::move(tuple);
        }

    private:
        TupleExpression(const TupleExpression &old_expr, std::shared_ptr<Scope> scope)
                : Expression(std::move(scope), "TupleExpression")
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

        std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> obj_ref) override;

        std::string toString() const override;

        std::unique_ptr<Expression> clone() override;

        bool isSpecialForm();

        virtual std::unique_ptr<Expression> call(expression_vector args);

        explicit FunctionExpression(std::string funcName,
                                    std::function<std::unique_ptr<Expression>(expression_vector,
                                                                              std::shared_ptr<Scope>)> &func,
                                    std::shared_ptr<Scope> scope)
                : Expression(std::move(scope), "FunctionExpression")
        {
            mFuncName = std::move(funcName);
            mFunction = func;
        }

    protected:
        explicit FunctionExpression(std::shared_ptr<Scope> scope) : Expression(std::move(scope), "FunctionExpression")
        {}

        bool mSpecialForm = false;
        std::string mFuncName;
        std::function<std::unique_ptr<Expression>(expression_vector, std::shared_ptr<Scope>)> mFunction;

    private:
        FunctionExpression(const FunctionExpression &old_expr, std::shared_ptr<Scope> scope)
                : Expression(std::move(scope), "FunctionExpression")
        {
            this->mFuncName = old_expr.mFuncName;
            this->mFunction = old_expr.mFunction;
        }
    };

    class SpecialFormExpression : public FunctionExpression
    {
    public:
        explicit SpecialFormExpression(std::string funcName,
                                       std::function<std::unique_ptr<Expression>(expression_vector,
                                                                                 std::shared_ptr<Scope>)> &func,
                                       std::shared_ptr<Scope> scope)
                : FunctionExpression(std::move(funcName), func, std::move(scope))
        {
            mSpecialForm = true;
        }
    };

    class LambdaExpression : public FunctionExpression
    {
    public:
        std::unique_ptr<Expression> call(expression_vector args) override;

        std::unique_ptr<Expression> clone() override;

        /* lambda_expr should have "lambda" at front(), and it should have the arg tuple */
        explicit LambdaExpression(std::vector<std::string> lambda_expr,
                                  std::vector<std::string> lambda_args, std::shared_ptr<Scope> scope)
                : FunctionExpression(std::move(scope))
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

            mSpecialForm = true;
        }

    protected:
        std::vector<std::string> mLambdaArgs;
        std::vector<std::string> mLambdaExpr;

    private:
        LambdaExpression(const LambdaExpression &old_expr, std::shared_ptr<Scope> scope)
                : FunctionExpression(std::move(scope))
        {
            this->mLambdaArgs = old_expr.mLambdaArgs;
            this->mLambdaExpr = old_expr.mLambdaExpr;
            this->mFuncName = old_expr.mFuncName;
            this->mFunction = old_expr.mFunction;
        }
    };

    class NumericalValueExpression : public Expression
    {
    public:
        boost::rational<int> mValue;

        bool isValue() override;

        std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> obj_ref) override;

        std::string toString() const override;

        std::unique_ptr<Expression> clone() override;

        explicit NumericalValueExpression(const std::string &str, std::shared_ptr<Scope> scope)
                : Expression(std::move(scope), "NumericalValueExpression")
        {
            if (str.find('/') != std::string::npos)
            {
                bool slashPassed = false;
                int numerator = 0, denominator = 0;
                for (int i = 0; i < str.length(); ++i)
                {
                    if (str[i] == '/' && !slashPassed)
                    {
                        slashPassed = true;
                    }
                    else if (str[i] == '/') throw std::invalid_argument("Parsing Error on " + str);
                    else if (isdigit(str[i]))
                    {
                        if (slashPassed)
                        {
                            denominator *= 10;
                            denominator += str[i] - '0';
                        }
                        else
                        {
                            numerator *= 10;
                            numerator += str[i] - '0';
                        }
                    }
                }

                mValue = boost::rational<int>(numerator, denominator);
            }
            else
            {
                double value = std::stod(str);

                int denominator = 1;

                while (value - (int) value != 0)
                {
                    value *= 10;
                    denominator *= 10;
                }

                mValue = boost::rational<int>((int) value, denominator);
            }
        }

        explicit NumericalValueExpression(boost::rational<int> value, std::shared_ptr<Scope> scope)
                : Expression(std::move(scope), "NumericalValueExpression")
        {
            mValue = value;
        }

        explicit NumericalValueExpression(int numerator, int denominator, std::shared_ptr<Scope> scope)
                : Expression(std::move(scope), "NumericalValueExpression")
        {
            mValue = boost::rational<int>(numerator, denominator);
        }

        explicit NumericalValueExpression(double value, std::shared_ptr<Scope> scope)
                : Expression(std::move(scope), "NumericalValueExpression")
        {
            int denominator = 1;

            while (value - (int) value != 0)
            {
                value *= 10;
                denominator *= 10;
            }

            mValue = boost::rational<int>((int) value, denominator);
        }

    private:
        NumericalValueExpression(const NumericalValueExpression &old_expr, std::shared_ptr<Scope> scope)
                : Expression(std::move(scope), "NumericalValueExpression")
        {
            this->mValue = old_expr.mValue;
        }
    };

    class InexactNumberExpression : public Expression
    {
    public:
        double value;

        bool isValue() override;

        std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> obj_ref) override;

        std::string toString() const override;

        std::unique_ptr<Expression> clone() override;

        explicit InexactNumberExpression(double value, std::shared_ptr<Scope> scope)
                : Expression(std::move(scope), "InexactNumberExpression")
        {
            this->value = value;
        }
    };

    class VoidValueExpression : public Expression
    {
    public:
        bool isValue() override;

        std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> obj_ref) override;

        std::string toString() const override;

        std::unique_ptr<Expression> clone() override;

        explicit VoidValueExpression(std::shared_ptr<Scope> scope) : Expression(std::move(scope), "VoidValueExpression")
        {}
    };

    // Non-numerical value list:
    // TODO: Lists

    class BooleanValueExpression : public Expression
    {
    public:
        bool value = false;

        bool isValue() override;

        std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> obj_ref) override;

        std::string toString() const override;

        std::unique_ptr<Expression> clone() override;

        explicit BooleanValueExpression(bool val, std::shared_ptr<Scope> scope)
                : Expression(std::move(scope), "BooleanValueExpression")
        {
            this->value = val;
        }
    };

    class SymbolExpression : public Expression
    {
    public:
        std::string symbol;

        bool isValue() override;

        std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> obj_ref) override;

        std::string toString() const override;

        std::unique_ptr<Expression> clone() override;

        explicit SymbolExpression(const std::string &symbol, std::shared_ptr<Scope> scope)
                : Expression(std::move(scope), "SymbolExpression")
        {
            this->symbol = symbol;
        }
    };
}

#endif //RACKET_INTERPRETER_EXPRESSIONS_H
