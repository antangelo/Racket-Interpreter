//
// Created by antonio on 27/12/18.
//

#include <iostream>

#include "parser.h"
#include "../functions/functions.h"

namespace Parser
{
    /**
    * Gives the ending index of the first occurring tuple in the given string
    * @param tuple A string where the first character is parenOpen and containing a closing parenClose somewhere.
    * @return The index of the parenClose closing the tuple opened by the first character, or 0 if it cannot be found.
    */
    size_t findTupleEnd(const std::string &tuple)
    {
        int tupleCount = 0;
        bool inString = false;
        if (tuple[0] != '(' && tuple[0] != '[') return 0;

        for (size_t index = 0; index < tuple.size(); ++index)
        {
            char chr = tuple[index];

            if (chr == '"' && index > 0 && tuple[index - 1] != '\\') inString = !inString;
            if (inString) continue;

            if (chr == '(' || chr == '[') tupleCount++;
            else if (chr == ')' || chr == ']') tupleCount--;

            if (tupleCount == 0) return index;
        }

        return std::string::npos;
    }

    std::vector<std::string> parseTuple(const std::string &str)
    {
        int nestedTupleCount = 0;
        bool inString = false;
        std::vector<std::string> rtn;
        std::string elem;

        for (int index = 0; index < str.length(); ++index)
        {
            char chr = str[index];

            if (chr == '"' && index >= 1 && str[index - 1] != '\\')
                inString = !inString;

            if (inString)
            {
                elem += chr;
                continue;
            }

            if (nestedTupleCount == 1 && (chr == ')' || chr == ']'))
            {
                if (!elem.empty()) rtn.push_back(elem);

                return rtn;
            }

            if (chr == ')' || chr == ']')
            {
                --nestedTupleCount;

                // No need for the continue shown below,
                // The above condition deals with it for us.
                // It will skip the end parenthesis if it is the last one.
            }
            else if (chr == '(' || chr == '[')
            {
                ++nestedTupleCount;

                if (nestedTupleCount <= 1) continue;
            }
            else if (nestedTupleCount == 1 && (chr == ' ' || chr == '\n'))
            {
                if (elem.empty()) continue;

                rtn.push_back(elem);
                elem = "";
                continue;
            }

            elem += chr;
        }

        throw std::invalid_argument("Tuple \"" + str + "\" is never closed.");
    }

    /**
     * Limits string contents to be between quotes
     * The first character in the string must be a quote
     */
    std::string parseString(const std::string &str)
    {
        std::string rtn;
        bool inString = false;

        for (int index = 0; index < str.length(); ++index)
        {
            char chr = str[index];
            if (chr == '"' && ((index >= 1 && str[index - 1] != '\\') || index == 0))
                inString = !inString;
            else if (inString) rtn += chr;
            else break;
        }

        return rtn;
    }

    std::string parseSymbol(const std::string &str)
    {
        if (str.length() <= 1) throw std::invalid_argument("Invalid symbol: " + str);

        std::string rtn;
        bool inPipe = false;

        for (auto &chr : str)
        {
            if (inPipe && chr == '|')
            {
                rtn += chr;
                break;
            }
            else if (chr == '|') inPipe = !inPipe;
            else if (!inPipe && (chr == ' ' || chr == '\n' || chr == '\t')) break;

            rtn += chr;
        }

        return rtn;
    }

    /**
     * Replaces instances of key in str with rpl within key's scope.
     * @param str The string to replace keys, should be a tuple
     * @param key The key to replace
     * @param rpl The string to replace key with
     */
    void replaceInScope(std::string &str, const std::string &key, const std::string &rpl)
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
            }
            else out += s;

            if (i < tuple.size() - 1) out += " ";
        }

        out += ")";
        str = out;
    }

    void parseSpecialForm(const std::string &str, const std::shared_ptr<Expressions::Scope> &scope,
                          std::unique_ptr<Expressions::Expression> &out)
    {
        if (Functions::specialFormMap.count(str) > 0)
        {
            out = Functions::getFormByName(str, scope);
        }
        else
        {
            std::unique_ptr<Expressions::Scope> localScope(new Expressions::Scope(scope));
            out = std::unique_ptr<Expressions::Expression>(
                    new Expressions::UnparsedExpression(str, std::move(localScope)));
        }
    }

    std::unique_ptr<Expressions::Expression> parse(std::string str, const std::shared_ptr<Expressions::Scope> &scope)
    {
        if (str[0] == '(' || str[0] == '[')
        {
            std::shared_ptr<Expressions::Scope> localScope(new Expressions::Scope(scope));
            std::unique_ptr<Expressions::Expression> expr(new Expressions::PartialExpression(parseTuple(str),
                                                                                             std::move(localScope)));
            return std::move(expr);
        }
        else if (str[0] == '"')
        {
            std::shared_ptr<Expressions::Scope> localScope(new Expressions::Scope(scope));
            return std::make_unique<Expressions::StringExpression>
                    (Expressions::StringExpression(parseString(str), std::move(localScope)));
        }
        else if (str[0] == '#')
        {
            std::shared_ptr<Expressions::Scope> localScope(new Expressions::Scope(scope));
            std::unique_ptr<Expressions::Expression> expr(new Expressions::VoidValueExpression(localScope));
            return std::move(expr);
        }
        else if (str == "true")
        {
            std::shared_ptr<Expressions::Scope> localScope(new Expressions::Scope(scope));
            return std::unique_ptr<Expressions::Expression>(
                    new Expressions::BooleanValueExpression(true, std::move(localScope)));
        }
        else if (str == "false")
        {
            std::shared_ptr<Expressions::Scope> localScope(new Expressions::Scope(scope));
            return std::unique_ptr<Expressions::Expression>(
                    new Expressions::BooleanValueExpression(false, std::move(localScope)));
        }
        else if (isdigit(str[0]) || (str[0] == '-' && isdigit(str[1])))
        {
            try
            {
                std::unique_ptr<Expressions::Scope> localScope(new Expressions::Scope(scope));
                std::unique_ptr<Expressions::Expression> expr(
                        new Expressions::NumericalValueExpression(str, std::move(localScope)));
                return std::move(expr);
            }
            catch (std::invalid_argument &exception)
            {
                std::cerr << "Unable to parse number " + str << std::endl;
                std::cerr << exception.what() << std::endl;
            }
        }
        else if (str[0] == '\'')
        {
            std::shared_ptr<Expressions::Scope> localScope(new Expressions::Scope(scope));
            return std::make_unique<Expressions::SymbolExpression>(
                    Expressions::SymbolExpression(parseSymbol(str), std::move(localScope)));
        }
        else if (scope != nullptr && scope->contains(str))
        {
            return scope->getDefinition(str);
        }
        else if (Functions::funcMap.count(str) > 0)
        {
            return Functions::getFuncByName(str, scope);
        }
        else if (Functions::specialFormMap.count(str) > 0)
        {
            return Functions::getFormByName(str, scope);
        }

        throw std::invalid_argument("Invalid expression: " + str);
    }
}
