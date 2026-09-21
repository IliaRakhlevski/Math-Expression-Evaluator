/**
 * @file Main.cpp
 * @brief Console front end.
 *
 * Two modes:
 *  - one-shot:    math_eval "2 * (3 + 4)"   prints the value, exit code 0/1;
 *  - interactive: math_eval                 reads one expression per line
 *                                           until an empty line, "quit" or EOF.
 */
#include <iomanip>
#include <iostream>
#include <string>

#include "Calculator.h"

namespace
{
	/**
	 * @brief Print "= value" or "Error: reason".
	 * @return true on success.
	 */
	bool print_result(const std::string& expression)
	{
		const CalcResult result = calculate(expression);

		if (result.ok())
			std::cout << "= " << result.value << '\n';
		else
			std::cout << "Error: " << to_message(result.error) << '\n';

		return result.ok();
	}
}


int main(int argc, char* argv[])
{
	// 12 significant digits: enough to be useful, short enough to hide
	// binary noise such as 0.1 + 0.2 = 0.30000000000000004.
	std::cout << std::setprecision(12);

	if (argc > 1)
	{
		// The shell may split an unquoted expression; glue the parts back.
		std::string expression;
		for (int i = 1; i < argc; ++i)
		{
			if (i > 1)
				expression += ' ';
			expression += argv[i];
		}
		return print_result(expression) ? 0 : 1;
	}

	std::cout << "Enter an expression (empty line or \"quit\" to exit).\n";

	std::string line;
	while (std::cout << "> " && std::getline(std::cin, line))
	{
		// Input piped from a Windows file may keep its '\r'.
		if (!line.empty() && line.back() == '\r')
			line.pop_back();

		if (line.empty() || line == "quit" || line == "exit")
			break;

		print_result(line);
	}

	return 0;
}
