//
// Created by antonio on 27/12/18.
//

#ifndef RACKET_INTERPRETER_PARSER_H
#define RACKET_INTERPRETER_PARSER_H

#include <string>
#include <list>

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

    protected:
        explicit Expression() = default;
    };

    typedef std::list<std::unique_ptr<Expression>> expression_list;

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
        expression_list mTupleMembers;

        bool isValue() override;

        std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> *obj_ref) override;

        std::string toString() const override;

        explicit TupleExpression(expression_list tuple)
        {
            mTupleMembers = std::move(tuple); //TODO: Re-implement to avoid double move call
        }
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

namespace Parser
{
    bool parse(std::string, std::unique_ptr<Expressions::Expression> &);
}

#endif //RACKET_INTERPRETER_PARSER_H
