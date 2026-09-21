#pragma once
#include <iostream>

/**
 * @file check.h
 * @brief Minimal test helper: no framework, one macro and a summary.
 *
 * CHECK(cond) prints the failed expression with file and line and keeps
 * going, so one run reports every failure. report() returns the exit code.
 */

namespace test
{
	inline int& failures()
	{
		static int count = 0;
		return count;
	}

	inline int& checks()
	{
		static int count = 0;
		return count;
	}

	inline int report()
	{
		std::cout << checks() - failures() << '/' << checks() << " checks passed\n";
		return failures() == 0 ? 0 : 1;
	}
}

#define CHECK(cond)                                                          \
	do {                                                                     \
		++test::checks();                                                    \
		if (!(cond)) {                                                       \
			++test::failures();                                              \
			std::cout << __FILE__ << ':' << __LINE__ << ": FAILED: " #cond "\n"; \
		}                                                                    \
	} while (false)
