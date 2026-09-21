#pragma once
#include <stack>
#include <vector>
#include <map>
#include "Tokenizer.h"


enum class Precedence { LOW, MID, HIGH, UNKNOWN };

enum class ComparePrecedenceRes { LESS, EQUAL, GREATER, UNKNOWN };

enum class Associative { RIGHT, LEFT, UNKNOWN };


class ShuntingYard
{
	std::vector<Token> tokens;
	std::map<TokenType, std::pair<Precedence, Associative>> operator_precedence_associative;

	std::stack<Token> op_stack;
	std::vector<Token> rpn;

	bool is_operator(const Token& token);
	Precedence get_precedence(const Token& token);
	Associative get_associative(const Token& token);
	ComparePrecedenceRes compare_precedences(const Token& token_a, const Token& token_b);
	void process_operator(Token& token);
	void process_right_paren();

public:
	ShuntingYard() = delete;
	ShuntingYard(const Tokenizer& tokenizer);

	void create_rpn();

	std::vector<Token> get_rpn() const;
};