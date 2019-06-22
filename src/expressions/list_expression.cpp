//
// Created by Antonio Abbatangelo on 2019-06-22.
//

#include "expressions.h"

namespace Expressions
{
    bool ListExpression::isValue()
    {
        return true;
    }

    std::string ListExpression::toString() const
    {
        if (this->list.empty()) return "empty";

        std::string rtn = "(list";

        for (auto &elem : this->list)
        {
            rtn += " ";
            rtn += elem->toString();
        }

        return rtn + ")";
    }

    std::unique_ptr<Expression> ListExpression::evaluate(std::unique_ptr<Expressions::Expression> obj_ref)
    {
        return std::move(obj_ref);
    }

    std::unique_ptr<Expression> ListExpression::clone()
    {
        std::list<std::unique_ptr<Expression>> listClone;

        for (auto &elem : this->list)
        {
            listClone.push_front(elem->clone());
        }

        return std::make_unique<ListExpression>(ListExpression(std::move(listClone), localScope));
    }
}