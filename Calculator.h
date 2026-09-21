#pragma once
#include <string>

/**
 * @file Calculator.h
 * @brief One-call entry point: expression text in, value or error out.
 *
 * The pipeline has four steps that must run in a fixed order:
 * tokenize -> validate -> mark unary minus -> Shunting Yard -> evaluate RPN.
 * Calling them by hand is easy to get wrong (skipping the unary-minus step
 * makes "-3" fail), so this facade is the only thing callers need.
 */

/**
 * @brief Why an expression could not be evaluated.
 */
enum class CalcError
{
	NONE,                   ///< Success.
	EMPTY_EXPRESSION,       ///< Nothing but spaces.
	INVALID_CHAR,           ///< A character outside "0-9 . + - * / ( ) space".
	INVALID_NUMBER,         ///< A malformed number such as "1.2.3" or ".".
	UNBALANCED_PARENTHESIS, ///< Parentheses do not pair up.
	INVALID_EXPRESSION,     ///< Tokens in an illegal order, e.g. "2 3" or "(*".
	DIVISION_BY_ZERO        ///< The divisor evaluated to exactly zero.
};

/**
 * @brief Result of calculate(): a value, or the reason there is none.
 */
struct CalcResult
{
	double value = 0.0;             ///< Meaningful only when ok() is true.
	CalcError error = CalcError::NONE;

	bool ok() const { return error == CalcError::NONE; }
};

/**
 * @brief Evaluate an infix expression.
 * @param expression E.g. "-(2 + 3) * 4 / .5".
 * @return The value, or the first error found.
 */
CalcResult calculate(const std::string& expression);

/**
 * @brief Human-readable text for an error code.
 */
const char* to_message(CalcError error);
