#include <iostream>
#include "Tokenizer.h"
#include "ShuntingYard.h"
#include "Evaluator.h"



int main()
{
	std::string exp = "-(-3.4 * (-2.3+4.85) + (-1.23)) / .89";

	std::cout << "Expression: " << exp << std::endl;

	Tokenizer tokenizer;

	TokenRetValue ret = tokenizer.tokenize(exp);
	if (ret == TokenRetValue::INVALID_CHAR)
	{
		std::cout << "Invalid character" << std::endl;
		return 0;
	}
	else if (ret == TokenRetValue::INVALID_NUMBER)
	{
		std::cout << "Invalid number" << std::endl;
		return 0;
	}
	else if (ret == TokenRetValue::UNBALANCED_PARENTHESIS)
	{
		std::cout << "Unbalanced parenthesis" << std::endl;
		return 0;
	}

	ret = tokenizer.validate();
	if (ret == TokenRetValue::MATH_EXP_NOT_TOKENIZED)
	{
		std::cout << "Math expression is not tokenized" << std::endl;
		return 0;
	}
	else if (ret == TokenRetValue::INVALID_MATH_EXP)
	{
		std::cout << "Invalid expression" << std::endl;
		return 0;
	}
	else
		std::cout << "Correct math expression" << std::endl;

	tokenizer.update_unary_minus();

	ShuntingYard shunting_yard(tokenizer);
	shunting_yard.create_rpn();


	std::vector<Token> rpn = shunting_yard.get_rpn();

	bool is_error;
	double val = evaluate_rpn(rpn, is_error);

	if (!is_error)
		std::cout << "Evaluation result: " << val << std::endl;
	else
		std::cout << "Evaluation error!" << std::endl;

	return 0;
}