#pragma once
#include <vector>
#include "Tokenizer.h"

double evaluate_rpn(const std::vector<Token>& rpn, bool& error);