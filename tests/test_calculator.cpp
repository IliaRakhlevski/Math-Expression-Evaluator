/**
 * @file test_calculator.cpp
 * @brief Tests for the whole pipeline and for the Shunting Yard step alone.
 */
#include <cmath>
#include <string>
#include <vector>

#include "check.h"
#include "../Calculator.h"
#include "../Tokenizer.h"
#include "../ShuntingYard.h"

namespace
{
	/// Relative tolerance: results pass through binary floating point.
	bool near(double actual, double expected)
	{
		return std::fabs(actual - expected) <= 1e-12 * (1.0 + std::fabs(expected));
	}

	bool gives(const std::string& expression, double expected)
	{
		const CalcResult r = calculate(expression);
		return r.ok() && near(r.value, expected);
	}

	bool fails_with(const std::string& expression, CalcError expected)
	{
		return calculate(expression).error == expected;
	}

	/// Render RPN as text, e.g. "1 2 3 * +", to compare with a known answer.
	std::string rpn_of(const std::string& expression)
	{
		Tokenizer tokenizer;
		tokenizer.tokenize(expression);
		tokenizer.update_unary_minus();
		ShuntingYard shunting_yard(tokenizer);
		shunting_yard.create_rpn();

		std::string text;
		for (const Token& token : shunting_yard.get_rpn())
		{
			if (!text.empty())
				text += ' ';

			switch (Tokenizer::get_token_type(token))
			{
			case TokenType::NUMBER:      text += std::to_string(static_cast<int>(std::get<double>(token))); break;
			case TokenType::PLUS:        text += '+'; break;
			case TokenType::MINUS:       text += '-'; break;
			case TokenType::MULT:        text += '*'; break;
			case TokenType::DIV:         text += '/'; break;
			case TokenType::UNARY_MINUS: text += '~'; break;
			default:                     text += '?'; break;
			}
		}
		return text;
	}

	void test_values()
	{
		CHECK(gives("42", 42));
		CHECK(gives("1+2", 3));
		CHECK(gives("  1 +   2 ", 3));
		CHECK(gives(".5 + 1.", 1.5));
		CHECK(gives("10/4", 2.5));
		CHECK(gives("-(-3.4 * (-2.3+4.85) + (-1.23)) / .89", 9.9 / 0.89));
	}

	void test_precedence_and_associativity()
	{
		CHECK(gives("1+2*3", 7));
		CHECK(gives("(1+2)*3", 9));
		CHECK(gives("2-3-4", -5));     // left associative: (2-3)-4
		CHECK(gives("8/2/2", 2));      // left associative: (8/2)/2
		CHECK(gives("2*3/4", 1.5));
		CHECK(gives("((((7))))", 7));
	}

	void test_unary_minus()
	{
		CHECK(gives("-5", -5));
		CHECK(gives("-2*3", -6));
		CHECK(gives("-(2+3)", -5));
		CHECK(gives("4*(-2)", -8));
		CHECK(gives("-(-(-1))", -1));
		CHECK(gives("-3-2", -5));      // unary binds tighter than binary minus
	}

	void test_division()
	{
		CHECK(fails_with("1/0", CalcError::DIVISION_BY_ZERO));
		CHECK(fails_with("1/(2-2)", CalcError::DIVISION_BY_ZERO));
		CHECK(fails_with("0/0", CalcError::DIVISION_BY_ZERO));
		CHECK(gives("0/5", 0));
		// A tiny divisor is not zero; the old epsilon test wrongly rejected it.
		CHECK(gives("1/0.0000000000000001", 1e16));
	}

	void test_errors()
	{
		CHECK(fails_with("", CalcError::EMPTY_EXPRESSION));
		CHECK(fails_with("   ", CalcError::EMPTY_EXPRESSION));
		CHECK(fails_with("2+a", CalcError::INVALID_CHAR));
		CHECK(fails_with("1e3", CalcError::INVALID_CHAR));
		CHECK(fails_with("1.2.3", CalcError::INVALID_NUMBER));
		CHECK(fails_with(".", CalcError::INVALID_NUMBER));
		CHECK(fails_with("(1+2", CalcError::UNBALANCED_PARENTHESIS));
		CHECK(fails_with("1+2)", CalcError::UNBALANCED_PARENTHESIS));
		CHECK(fails_with(")(", CalcError::UNBALANCED_PARENTHESIS));
		CHECK(fails_with("2 3", CalcError::INVALID_EXPRESSION));
		CHECK(fails_with("()", CalcError::INVALID_EXPRESSION));
		CHECK(fails_with("1+", CalcError::INVALID_EXPRESSION));
		CHECK(fails_with("*1", CalcError::INVALID_EXPRESSION));
		CHECK(fails_with("(2)(3)", CalcError::INVALID_EXPRESSION));
		// Documented limitation: unary minus only at the start or after '('.
		CHECK(fails_with("2*-3", CalcError::INVALID_EXPRESSION));
		CHECK(fails_with("2--3", CalcError::INVALID_EXPRESSION));
	}

	void test_rpn()
	{
		CHECK(rpn_of("1+2*3") == "1 2 3 * +");
		CHECK(rpn_of("(1+2)*3") == "1 2 + 3 *");
		CHECK(rpn_of("1-2-3") == "1 2 - 3 -");
		CHECK(rpn_of("-(1+2)") == "1 2 + ~");
		CHECK(rpn_of("-1*2") == "1 ~ 2 *");
	}
}


int main()
{
	test_values();
	test_precedence_and_associativity();
	test_unary_minus();
	test_division();
	test_errors();
	test_rpn();
	return test::report();
}
