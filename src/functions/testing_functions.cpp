//
// Created by Antonio Abbatangelo on 2019-06-02.
//

#include <list>
#include "boost/multiprecision/cpp_dec_float.hpp"
#include "functions.h"
#include "../interpret/interpret.h"

namespace TestingFunctions
{
    struct TestCase
    {
        std::unique_ptr<Expressions::Expression> test, expected;
        boost::multiprecision::cpp_dec_float_100 within;
    };

    std::list<TestCase> testCases;

    std::unique_ptr<Expressions::Expression> check_expect_fn(Expressions::expression_vector expr,
                                                             std::shared_ptr<Expressions::Scope> scope)
    {
        if (expr.size() != 2) throw std::invalid_argument("check-expect: Expected 2 params");

        struct TestCase testCase = {std::move(expr[0]), std::move(expr[1]), 0};
        testCases.push_back(std::move(testCase));

        return std::unique_ptr<Expressions::Expression>
                (new Expressions::VoidValueExpression(
                        std::make_unique<Expressions::Scope>(Expressions::Scope(std::move(scope)))));
    }

    std::unique_ptr<Expressions::Expression> check_within_fn(Expressions::expression_vector expr,
                                                             std::shared_ptr<Expressions::Scope> scope)
    {
        if (expr.size() != 3) throw std::invalid_argument("check-within: Expected 3 params");
        expr[2] = Expressions::evaluate(std::move(expr[2]));
        Expressions::InexactNumberExpression::numerical_type within;

        if (auto r1 = dynamic_cast<Expressions::NumericalValueExpression *>(expr[2].get()))
            within = boost::rational_cast<boost::multiprecision::cpp_dec_float_100>(r1->value);
        else if (auto i1 = dynamic_cast<Expressions::InexactNumberExpression *>(expr[2].get()))
            within = i1->value;
        else throw std::invalid_argument("Expected number, got " + expr[2]->toString());

        struct TestCase testCase = {std::move(expr[0]), std::move(expr[1]), within};
        testCases.push_back(std::move(testCase));

        return std::unique_ptr<Expressions::Expression>
                (new Expressions::VoidValueExpression(
                        std::make_unique<Expressions::Scope>(Expressions::Scope(std::move(scope)))));
    }

    bool numbersWithin(const std::unique_ptr<Expressions::Expression> &test,
                       const std::unique_ptr<Expressions::Expression> &expected,
                       const Expressions::InexactNumberExpression::numerical_type &within)
    {
        Expressions::InexactNumberExpression::numerical_type n1, n2;

        if (auto r1 = dynamic_cast<Expressions::NumericalValueExpression *>(test.get()))
            n1 = boost::rational_cast<boost::multiprecision::cpp_dec_float_100>(r1->value);
        else if (auto i1 = dynamic_cast<Expressions::InexactNumberExpression *>(test.get()))
            n1 = i1->value;
        else throw std::invalid_argument("Expected number, got " + test->toString());

        if (auto r2 = dynamic_cast<Expressions::NumericalValueExpression *>(expected.get()))
            n2 = boost::rational_cast<boost::multiprecision::cpp_dec_float_100>(r2->value);
        else if (auto i2 = dynamic_cast<Expressions::InexactNumberExpression *>(expected.get()))
            n2 = i2->value;
        else throw std::invalid_argument("Expected number, got " + expected->toString());

        return boost::multiprecision::abs(n1 - n2) <= within;
    }

    std::unique_ptr<Expressions::Expression> run_tests_fn(const Expressions::expression_vector &expr,
                                                          std::shared_ptr<Expressions::Scope> scope)
    {
        if (!expr.empty()) throw std::invalid_argument("run-tests expects no args");
        int passedTests = 0, totalTests = 0;

        for (auto &testCase : testCases)
        {
            ++totalTests;

            std::cout << "Test case: " << testCase.test->toString() << " == " << testCase.expected->toString()
                      << std::endl;

            std::unique_ptr<Expressions::Expression> test = Interpreter::interpret(
                    Expressions::evaluate(std::move(testCase.test)));
            std::unique_ptr<Expressions::Expression> expected = Interpreter::interpret(
                    Expressions::evaluate(std::move(testCase.expected)));

            //TODO: Replace naive comparison
            if (testCase.within != 0 && numbersWithin(test, expected, testCase.within)) ++passedTests;
            else if (test->toString() == expected->toString()) ++passedTests;
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
}

void register_testing_functions()
{
    Functions::specialFormMap["check-expect"] = TestingFunctions::check_expect_fn;
    Functions::specialFormMap["check-within"] = TestingFunctions::check_within_fn;
    Functions::funcMap["run-tests"] = TestingFunctions::run_tests_fn;
}