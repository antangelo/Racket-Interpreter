//
// Created by antonio on 27/12/18.
//

#include <iostream>

#include "parser.h"

namespace Expressions
{
    std::ostream &operator<<(std::ostream &stream, const Expression &expr)
    {
        stream << expr.toString();
        return stream;
    }

    /* PartialExpression */

    bool PartialExpression::isValue()
    {
        return false;
    }

    std::unique_ptr<Expression> PartialExpression::evaluate(std::unique_ptr<Expression> *obj_ref)
    {
        Expressions::expression_list members;

        for (const auto &str : mTupleMembers)
        {
            std::unique_ptr<Expressions::Expression> expr;
            auto parseSuccessful = Parser::parse(str, expr);

            if (!parseSuccessful) throw std::invalid_argument; //TODO: Write exception for syntax errors/unsuccessful parsing.

            if (auto partialExpression = dynamic_cast<PartialExpression *>(expr.get()))
            {
                // Not the best, but obj_ref isn't used by evaluate() so it ends up saving
                // unnecessary pointer creation with a PartialExpression here.
                members.push_back(std::move(partialExpression->evaluate(nullptr)));
            } else
            {
                members.push_back(std::move(expr));
            }
        }

        std::unique_ptr<TupleExpression> expr(new TupleExpression(std::move(members)));
        return std::move(expr);
    }

    std::string PartialExpression::toString() const
    {
        std::string str = "(";

        for (const auto &i : mTupleMembers)
        {
            str += i + " ";
        }

        return str.substr(0, str.size() - 1) + ")";
    }

    /* TupleExpression */

    bool TupleExpression::isValue()
    {
        return false; // for now, will change in the future.
    }

    std::unique_ptr<Expression> TupleExpression::evaluate(std::unique_ptr<Expression> *obj_ref)
    {
        return std::move(*obj_ref);
    }

    std::string TupleExpression::toString() const
    {
        std::string str = "(";

        for (const auto &i : mTupleMembers)
        {
            str += i->toString() + " ";
        }

        return str.substr(0, str.size() - 1) + ")";
    }

    /* NumericalValueExpression */

    bool NumericalValueExpression::isValue()
    {
        return true;
    }

    std::unique_ptr<Expression> NumericalValueExpression::evaluate(std::unique_ptr<Expression> *obj_ref)
    {
        return std::move(*obj_ref);
    }

    std::string NumericalValueExpression::toString() const
    {
        return std::to_string(mValue);
    }
}

namespace Parser
{

/**
 * Gives the ending index of the first occurring tuple in the given string
 * @param tuple A string where the first character is a '(' and containing a closing ')' somewhere.
 * @return The index of the ')' closing the tuple opened by the first character, or 0 if it cannot be found.
 */
    size_t findTupleEnd(std::string tuple)
    {
        int tupleCount = 0;

        for (size_t index = 0; index < tuple.size(); index++)
        {
            if (tuple[index] == '(') tupleCount++;
            else if (tuple[index] == ')') tupleCount--;

            /* Based on the assumption that the first character is a '(',
             * this won't be true until we've found the end index*/
            if (tupleCount == 0) return index;
        }

        return std::string::npos;
    }

    /**
     *
     * @param str A string where the first character is '('
     * @return
     */
    std::list<std::string> parseTuple(std::string str)
    {
        /* Remove the opening parenthesis with erase */
        if (str[0] == '(') str.erase(0, 1);

        //std::string expr1 = str.substr(0, str.find(')'));
        std::list<std::string> tupleElements;

        /* Tokenize */
        size_t index = 0;
        while ((index = str.find(' ')) != std::string::npos)
        {
            if (str[0] == '(')
            {
                /* findTupleEnd produces the index of the ending ')', need to add
                 * one since substr is exclusive on the ending boundary. */
                index = findTupleEnd(str) + 1;
            }

            tupleElements.push_back(str.substr(0, index));
            str.erase(0, index + 1);
        }

        /* Remove the last character, since it'll be either a whitespace or ')' and is useless. */
        std::string remainder = str.substr(0, str.size() - 1);
        if (!remainder.empty()) tupleElements.push_back(remainder);

        return tupleElements;
    }

    bool parse(std::string str, std::unique_ptr<Expressions::Expression> &out)
    {
        if (str.front() == '('
            && findTupleEnd(str) != std::string::npos)
        {
            std::unique_ptr<Expressions::Expression> expr(new Expressions::PartialExpression(parseTuple(str)));
            out = std::move(expr);
            return true;
        } else
        {
            try
            {
                std::unique_ptr<Expressions::Expression> expr(
                        new Expressions::NumericalValueExpression(std::stod(str)));
                out = std::move(expr);
                return true;
            }
            catch (std::invalid_argument &exception)
            {
                std::cout << exception.what() << std::endl;
            }

            return false;
        }
    }
}
