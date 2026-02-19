#include <stack>
#include <limits>
#include <cmath>
#include "Evaluator.h"
using namespace std;



bool isAlmostZero(double value, double epsilon = numeric_limits<double>::epsilon()) {
	return fabs(value) < epsilon;
}


double perform_binary_operation(double left_val, double right_val, TokenType token_type, bool &is_error)
{
	double val = 0.0;
	is_error = false;
	switch (token_type)
	{
	case TokenType::PLUS:
		val = left_val + right_val;
		break;
	case TokenType::MINUS:
		val = left_val - right_val;
		break;
	case TokenType::MULT:
		val = left_val * right_val;
		break;
	case TokenType::DIV:
		if (!isAlmostZero(right_val))
			val = left_val / right_val;
		else
			is_error = true;
		break;
	default:
		is_error = true;
	}

	return val;
}


double evaluate_rpn(const vector<Token> &rpn, bool &error)
{
	std::stack<double> value_stack;
	double res = 0.0;
	error = false;

	for (const Token token : rpn)
	{
		TokenType token_type = Tokenizer::get_token_type(token);

		switch (token_type)
		{
			case TokenType::NUMBER:
			{
				value_stack.push(get<0>(token));
				break;
			}
			case TokenType::UNARY_MINUS:
			{
				if (!value_stack.empty())
				{
					double val = value_stack.top();
					value_stack.pop();
					value_stack.push(-val);
				}
				else
				{
					error = true;
					return res;
				}
				break;
			}
			case TokenType::PLUS:
			case TokenType::MINUS:
			case TokenType::MULT:
			case TokenType::DIV:
			{
				if (value_stack.size() > 1)
				{
					double right_val = value_stack.top();
					value_stack.pop();

					double left_val = value_stack.top();
					value_stack.pop();

					double val = perform_binary_operation(left_val, right_val, token_type, error);
					if (error)
						return res;

					value_stack.push(val);
				}
				else
				{
					error = true;
					return res;
				}
				break;
			}

		} // end switch
	}

	if (value_stack.size() == 1)
	{
		res = value_stack.top();
	}
	else
		error = true;

	return res;
}