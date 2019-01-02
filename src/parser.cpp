//
// Created by antonio on 27/12/18.
//

#include <iostream>

#include "parser.h"

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

            /* Based on the assumption that the first character is a '(', this won't be true until we've found the end index
             * If the given string does not have the correct first character, it will terminate with 0 as it would if the end
             *  could not be found. */
            if (tupleCount == 0) return index;
        }

        return std::string::npos;
    }

    std::vector<std::string> parse(std::string str)
    {
        /* Remove the opening parenthesis with erase */
        if (str[0] == '(') str.erase(0, 1);

        //std::string expr1 = str.substr(0, str.find(')'));
        std::vector<std::string> vector1;

        /* Tokenize */
        size_t index = 0;
        while ((index = str.find(' ')) != std::string::npos)
        {
            if (str[0] == '(')
            {
                /* findTupleEnd produces the index of the ending ')', need to add one since substr is exclusive on the ending boundary. */
                index = findTupleEnd(str) + 1;
            }

            vector1.push_back(str.substr(0, index));
            str.erase(0, index + 1);
        }

        /* Remove the last character, since it'll be either a whitespace or ')' and is useless. */
        std::string remainder = str.substr(0, str.size() - 1);
        if (!remainder.empty()) vector1.push_back(remainder);

        return vector1;
    }

}
