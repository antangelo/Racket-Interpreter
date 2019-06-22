//
// Created by Antonio Abbatangelo on 2019-06-02.
//

#include <list>
#include "functions.h"
#include "../interpret/interpret.h"

struct TestCase
{
    std::unique_ptr<Expressions::Expression> test, expected;
};

std::list<TestCase> testCases;

std::unique_ptr<Expressions::Expression> check_expect_fn(Expressions::expression_vector expr,
                                                         std::shared_ptr<Expressions::Scope> scope)
{
    if (expr.size() != 2) throw std::invalid_argument("check-expect: Expected 2 params");

    struct TestCase testCase = {std::move(expr[0]), std::move(expr[1])};
    testCases.push_back(std::move(testCase));

    return std::unique_ptr<Expressions::Expression>
            (new Expressions::VoidValueExpression(
                    std::make_unique<Expressions::Scope>(Expressions::Scope(std::move(scope)))));
}

std::unique_ptr<Expressions::Expression> run_tests_fn(const Expressions::expression_vector &expr,
                                                      std::shared_ptr<Expressions::Scope> scope)
{
    if (!expr.empty()) throw std::invalid_argument("run-tests expects no args");
    int passedTests = 0, totalTests = 0;

    for (auto &testCase : testCases)
    {
        ++totalTests;

        std::cout << "Test case: " << testCase.test->toString() << " == " << testCase.expected->toString() << std::endl;

        std::unique_ptr<Expressions::Expression> test = Interpreter::interpret(
                Expressions::evaluate(std::move(testCase.test)));
        std::unique_ptr<Expressions::Expression> expected = Interpreter::interpret(
                Expressions::evaluate(std::move(testCase.expected)));

        //TODO: Replace naive comparison
        if (test->toString() == expected->toString()) ++passedTests;
        else
        {
            std::cout << "Test failed- Expected: " << expected->toString() << ", got: "
                      << test->toString() << std::endl;
        }
    }

    testCases.clear();
    std::cout << "Passed " << passedTests << " of " << totalTests << " test(s)." << std::endl;

    return std::unique_ptr<Expressions::Expression>
            (new Expressions::VoidValueExpression(
                    std::make_unique<Expressions::Scope>(Expressions::Scope(std::move(scope)))));
}

void register_testing_functions()
{
    Functions::specialFormMap["check-expect"] = check_expect_fn;
    Functions::funcMap["run-tests"] = run_tests_fn;
}