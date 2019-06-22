//
// Created by Antonio Abbatangelo on 2019-06-21.
//

#include "../functions/functions.h"
#include "../interpret/parser.h"
#include "struct_expression.h"

typedef std::unique_ptr<Expressions::Expression> expr_ptr;
typedef std::shared_ptr<Expressions::Scope> scope_ptr;
using Expressions::expression_vector;

typedef std::function<std::unique_ptr<Expressions::Expression>(expression_vector,
                                                               std::shared_ptr<Expressions::Scope>)> racket_function;

namespace Expressions
{
    bool StructExpression::isValue()
    {
        return true;
    }

    std::string StructExpression::toString() const
    {
        std::string rtn = "(make-" + this->structName;

        for (auto &field : this->structFields)
        {
            rtn += " " + field->toString();
        }

        return rtn + ")";
    }

    std::unique_ptr<Expression> StructExpression::evaluate(std::unique_ptr<Expressions::Expression> obj_ref)
    {
        return std::move(obj_ref);
    }

    std::unique_ptr<Expression> StructExpression::clone()
    {
        std::vector<std::unique_ptr<Expression>> fieldClone;

        for (auto &field : this->structFields)
        {
            fieldClone.push_back(field->clone());
        }

        return std::make_unique<StructExpression>
                (StructExpression(this->structName, std::move(fieldClone), this->localScope));
    }
}

namespace StructFunctions
{

    racket_function makeStructFn(const std::string &structName, int fieldCount)
    {
        return [structName, fieldCount](expression_vector args, scope_ptr scope) -> expr_ptr
        {
            Functions::arg_count_check(args, fieldCount);
            std::vector<expr_ptr> fields;
            fields.reserve(args.size());

            for (int i = 0; i < fieldCount; ++i)
            {
                fields.push_back(std::move(args[i]));
            }

            return std::make_unique<Expressions::StructExpression>
                    (Expressions::StructExpression(structName, std::move(fields), std::move(scope)));
        };
    }

    racket_function structPredicateFn(const std::string &structName)
    {
        return [structName](expression_vector args, scope_ptr scope) -> expr_ptr
        {
            Functions::arg_count_check(args, 1);
            bool rtn;

            if (auto structure = dynamic_cast<Expressions::StructExpression *>(args[0].get()))
            {
                rtn = structure->structName == structName;
            }
            else rtn = false;

            return std::make_unique<Expressions::BooleanValueExpression>
                    (Expressions::BooleanValueExpression(rtn, std::move(scope)));
        };
    }

    racket_function getStructFieldFn(const std::string &structName, int fieldNum)
    {
        return [structName, fieldNum](expression_vector args, scope_ptr scope) -> expr_ptr
        {
            Functions::arg_count_check(args, 1);

            if (auto structure = dynamic_cast<Expressions::StructExpression *>(args[0].get()))
            {
                if (structure->structName != structName)
                    throw std::invalid_argument("Expected " + structName + ", found " + structure->structName);

                return structure->structFields[fieldNum]->clone();
            }

            throw std::invalid_argument("Expected " + structName + ", found " + args[0]->toString());
        };
    }

    void defineStruct(const std::string &structName, const std::vector<std::string> &structFields)
    {
        Functions::funcMap["make-" + structName] = makeStructFn(structName, structFields.size());
        Functions::funcMap[structName + "?"] = structPredicateFn(structName);

        int fieldCount = 0;
        for (auto &fieldName : structFields)
        {
            std::stringstream getterName;
            getterName << structName << "-" << fieldName;
            Functions::funcMap[getterName.str()] = getStructFieldFn(structName, fieldCount);
            ++fieldCount;
        }
    }

    expr_ptr defineStructFn(expression_vector args, scope_ptr scope)
    {
        Functions::arg_count_check(args, 2);

        std::string structName = args[0]->toString();
        std::string structFieldTuple = args[1]->toString();
        std::vector<std::string> structFields = Parser::parseTuple(structFieldTuple);

        defineStruct(structName, structFields);

        return std::make_unique<Expressions::VoidValueExpression>
                (Expressions::VoidValueExpression(std::move(scope)));
    }
}

void register_struct_functions()
{
    Functions::specialFormMap["define-struct"] = StructFunctions::defineStructFn;
}