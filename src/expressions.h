//
// Created by Antonio on 2019-01-09.
//

#ifndef RACKET_INTERPRETER_EXPRESSIONS_H
#define RACKET_INTERPRETER_EXPRESSIONS_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <functional>

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
        virtual std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> *obj_ref) = 0;

        virtual std::string toString() const = 0;

        friend std::ostream &operator<<(std::ostream &stream, const Expression &expr);

        virtual ~Expression() = default;

    protected:
        explicit Expression() = default;
    };

    typedef std::vector<std::unique_ptr<Expression>> expression_vector;

    class PartialExpression : public Expression
    {
    public:
        std::list<std::string> mTupleMembers;

        bool isValue() override;

        std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> *obj_ref) override;

        std::string toString() const override;

        explicit PartialExpression(std::list<std::string> tuple)
        {
            mTupleMembers = std::move(tuple);
        }
    };

    class TupleExpression : public Expression
    {
    public:
        expression_vector mTupleMembers;

        bool isValue() override;

        std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> *obj_ref) override;

        std::string toString() const override;

        explicit TupleExpression(expression_vector tuple)
        {
            mTupleMembers = std::move(tuple); //TODO: Re-implement to avoid double move call
        }
    };

    class FunctionExpression : public Expression
    {
    public:
        bool isValue() override;

        std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> *obj_ref) override;

        std::string toString() const override;

        std::unique_ptr<Expression> call(expression_vector args);

        explicit FunctionExpression(std::string funcName,
                                    std::function<std::unique_ptr<Expression>(expression_vector)> &func)
        {
            mFuncName = std::move(funcName);
            mFunction = func;
        }

    private:
        std::string mFuncName;
        std::function<std::unique_ptr<Expression>(expression_vector)> mFunction;
    };

    class NumericalValueExpression : public Expression // TODO: Implement non-numerical values.
    {
    public:
        double mValue;

        bool isValue() override;

        std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> *obj_ref) override;

        std::string toString() const override;

        explicit NumericalValueExpression(double value)
        {
            mValue = value;
        }
    };
}

#endif //RACKET_INTERPRETER_EXPRESSIONS_H
