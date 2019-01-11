//
// Created by antonio on 27/12/18.
//

#include <iostream>

#include "parser.h"
#include "functions.h"

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
    std::vector<std::string> parseTuple(std::string str)
    {
        /* Remove the opening parenthesis with erase */
        if (str[0] == '(' || str[0] == '[') str.erase(0, 1);

        //std::string expr1 = str.substr(0, str.find(')'));
        std::vector<std::string> tupleElements;

        /* Tokenize */
        size_t index = 0;
        while ((index = str.find(' ')) != std::string::npos)
        {
            if (str[0] == '(' || str[0] == '[')
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

    /**
     * Replaces instances of key in str with rpl within key's scope.
     * @param str The string to replace keys, should be a tuple
     * @param key The key to replace
     * @param rpl The string to replace key with
     */
    void replaceInScope(std::string &str, std::string key, std::string rpl)
    {
        if (str == key)
        {
            str = rpl;
            return;
        }

        if (str[0] != '(')
        {
            return;
        }

        std::vector<std::string> tuple = parseTuple(str);
        std::string out = "(";

        for (int i = 0; i < tuple.size(); i++)
        {
            auto s = tuple[i];

            if (s == key) out += rpl;
            else if (s.front() == '(')
            {
                replaceInScope(s, key, rpl);
                out += s;
            } else out += s;

            if (i < tuple.size() - 1) out += " ";
        }

        out += ")";
        str = out;
    }

    bool parse(std::string str, std::unique_ptr<Expressions::Expression> &out)
    {
        if ((str.front() == '(' || str.front() == '[')
            && findTupleEnd(str) != std::string::npos)
        {
            std::unique_ptr<Expressions::Expression> expr(new Expressions::PartialExpression(parseTuple(str)));
            out = std::move(expr);
            return true;
        } else if (Functions::funcMap.count(str) > 0)
        {
            out = Functions::getFuncByName(str);
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
                //std::cout << exception.what() << std::endl;
                throw;
            }
        }
    }
}
