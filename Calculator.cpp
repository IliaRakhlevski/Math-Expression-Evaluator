#include "Calculator.h"
#include "Tokenizer.h"
#include "ShuntingYard.h"
#include "Evaluator.h"

namespace
{
	/**
	 * @brief Map a tokenizer/validator status to the public error code.
	 */
	CalcError from_token_status(TokenRetValue status)
	{
		switch (status)
		{
		case TokenRetValue::SUCCESS:                return CalcError::NONE;
		case TokenRetValue::INVALID_CHAR:           return CalcError::INVALID_CHAR;
		case TokenRetValue::INVALID_NUMBER:         return CalcError::INVALID_NUMBER;
		case TokenRetValue::UNBALANCED_PARENTHESIS: return CalcError::UNBALANCED_PARENTHESIS;
		case TokenRetValue::INVALID_MATH_EXP:       return CalcError::INVALID_EXPRESSION;
		case TokenRetValue::MATH_EXP_NOT_TOKENIZED: return CalcError::EMPTY_EXPRESSION;
		}
		return CalcError::INVALID_EXPRESSION;
	}
}


CalcResult calculate(const std::string& expression)
{
	Tokenizer tokenizer;

	TokenRetValue status = tokenizer.tokenize(expression);
	if (status == TokenRetValue::SUCCESS)
		status = tokenizer.validate();

	if (status != TokenRetValue::SUCCESS)
		return { 0.0, from_token_status(status) };

	tokenizer.update_unary_minus();

	ShuntingYard shunting_yard(tokenizer);
	shunting_yard.create_rpn();

	bool is_error = false;
	const double value = evaluate_rpn(shunting_yard.get_rpn(), is_error);

	// After validation the RPN is well formed, so the only way evaluation
	// can still fail is a zero divisor.
	if (is_error)
		return { 0.0, CalcError::DIVISION_BY_ZERO };

	return { value, CalcError::NONE };
}


const char* to_message(CalcError error)
{
	switch (error)
	{
	case CalcError::NONE:                   return "OK";
	case CalcError::EMPTY_EXPRESSION:       return "Empty expression";
	case CalcError::INVALID_CHAR:           return "Invalid character";
	case CalcError::INVALID_NUMBER:         return "Invalid number";
	case CalcError::UNBALANCED_PARENTHESIS: return "Unbalanced parentheses";
	case CalcError::INVALID_EXPRESSION:     return "Invalid expression";
	case CalcError::DIVISION_BY_ZERO:       return "Division by zero";
	}
	return "Unknown error";
}
